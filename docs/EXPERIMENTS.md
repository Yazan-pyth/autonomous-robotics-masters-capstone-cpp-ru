# Протокол экспериментов

Сокращения раскрыты при первом употреблении; [полный словарь терминов](GLOSSARY.md).

## До начала тестов

Зафиксировать исследовательские гипотезы, primary/secondary metrics, observation/action spaces, tuning budget, hardware и resource limits. Каждая группа выбирает одну primary metric из своей спецификации. Лучший метод не выбирают после просмотра test; если преимущество отсутствует, это полноценный результат.

Одинаковые sensors, карты, limits, hardware, safety monitor и test scenarios для обоих методов. Сравнение CPU (Central Processing Unit — центральный процессор) обязательно; GPU (Graphics Processing Unit — графический процессор) — отдельный режим с собственной таблицей. Указать стоимость обучения, число параметров, inference memory и время подготовки данных. Classical tuning также получает фиксированный validation budget. Измерять fallback fraction: learned configuration с постоянным fallback не доказывает качество модели.

## Разделение данных

В неделю 2 создать непересекающиеся семейства layout IDs (Identifiers — идентификаторы): `train_*`, `val_*`, `test_*`. Минимум 3 train layouts, 1 validation layout, 2 test layouts. Test worlds могут иметь общую спецификацию, но их logs/labels не используются для обучения или подбора. При нехватке ресурсов разрешены процедурные layouts одного генератора с disjoint parameters/seeds и явным ограничением выводов.

Нельзя разделять соседние video frames или один bag по строкам. Все кадры эпизода принадлежат одному split. Normalization/augmentation fit только на train. Mapping-pass test мира разрешён для общей navigation map; его данные не добавляются в обучающие выборки. Dataset hashes фиксируются; manifest хранит simulator seed отдельно от training seed.

Пример диапазонов: train scenario seeds 0–999, validation 1000–1099, test 2000–2019, OOD (Out of Distribution — данные или условия вне обучающего распределения) 3000–3019. Диапазоны сами по себе не обеспечивают отсутствие leakage — layout и trajectory split тоже обязательны. Перечень test episodes фиксируется до оценки; повторять только удобные seed запрещено.

## Три уровня сравнения

1. **Компонент:** replay или изолированная среда, одинаковые входы, A и B, минимум 20 test seeds/episodes на каждый выбранный тип сценария. Для G1–G3 обучить хотя бы один model seed и явно ограничить выводы о training variance. Для G4/G5 — **3 training seeds**, все показываются в isolated evaluation.
2. **Закрытый контур:** меняется один компонент, остальные фиксированы в classical configuration. Здесь проявляются effects на trajectory и downstream data distribution; входные trajectories уже не идентичны.
3. **Общая система:** семь конфигураций ниже. Learned checkpoint для каждого компонента выбирается только по validation и затем фиксируется на всю системную серию.

| ID (Identifier — идентификатор) | G1 | G2 | G3 | G4 | G5 | Что проверяем |
|---|---|---|---|---|---|---|
| C0 | A | A | A | A | A | Общий классический baseline |
| C1 | B | A | A | A | A | Вклад localization |
| C2 | A | B | A | A | A | Вклад perception |
| C3 | A | A | B | A | A | Вклад planning |
| C4 | A | A | A | B | A | Вклад control |
| C5 | A | A | A | A | B | Вклад dispatch |
| C6 | B | B | B | B | B | Совместимость всех обучаемых методов |

Необязательный C7 — выбранная по validation mixed system. Все 32 комбинации не требуются. Single replacement сравнения не раскрывают все взаимодействия; C6 помогает обнаружить их, но не обеспечивает полного factorial analysis.

## Общие сценарии и объём

Обязательны четыре семейства: S1 static nominal; S2 dynamic crossing; S3 blocked aisle/replan; S4 noise/slip/domain shift. В каждом — 20 test episodes, сбалансированных между двумя test layouts (10 + 10). Семь конфигураций × четыре семейства × 20 episodes = **560 системных прогонов**. Каждый episode до 300 с simulation time: верхняя оценка 46.7 ч simulation time; actual wall time и real-time factor измеряются. Smoke 2 seeds не заменяет final suite.

Дополнительно fault suite: sensor dropout, stale command, broken model, process exit и frozen clock; минимум 5 repeats каждого fault для C0 и C6. Fault runs не смешиваются с nominal success rate. Extra OOD suite маркируется отдельно и не подменяет четыре обязательных семейства. Fault onset задаётся в manifest, не вручную после наблюдения удобного момента.

## Метрики системы

- `success`: все released jobs завершены в срок эпизода, без collision/invalid termination. В основном benchmark finite workload выпускает все заявки до 180 с. Throughput suite с continuous arrivals — дополнительная, для неё эта success metric не применяется.
- `collision`: simulator contact robot–obstacle; контакт колёс с полом исключён. Повторные сообщения одного контакта объединяются, правило указывается.
- `timeout`: лимит 300 с достигнут при unfinished jobs без более раннего terminal failure. Все terminal reasons задаются ровно одним кодом; collision имеет приоритет перед timeout.
- `throughput = delivered / elapsed_sim_seconds * 3600`. При early collision дополнительно приводить `delivered / 300 * 3600`, чтобы раннее прекращение не создавало искусственного выигрыша.
- On-time fraction: число доставленных до due time / все released jobs; недоставленные не исключаются.
- Navigation SPL (Success weighted by Path Length — успешность навигации, взвешенная по эффективности длины пути) для отдельных start-goal legs: `S * L_shortest / max(L_shortest, L_actual)`, `S=0` для failed leg. `L_shortest` — по одной frozen static reference map с общим footprint; метрика не учитывает будущие dynamic obstacles, это ограничение явно указывается. Start=goal обрабатывается отдельно. Для миссии основной показатель — delivery, а не сумма SPL.
- Minimum clearance по GT (Ground Truth — эталонные данные для обучения или оценки) geometry, distance traveled, mean/p95 (95th percentile — 95-й процентиль) latency per component, peak memory, deadline misses, safety intervention count и fallback fraction.

ATE (Absolute Trajectory Error — абсолютная ошибка траектории): `sqrt(mean(||p_est-p_gt||²))` с time alignment/interpolation в общей карте. Tracking RMSE (Root Mean Square Error — среднеквадратическая ошибка) считается относительно active path segment, не ближайшего произвольного участка самопересекающегося пути. Latency percentiles — отдельно по component и hardware; не смешивать callback и end-to-end duration.

## Статистика и анализ

Единица анализа — episode, а не кадр. Показывать `n`, mean/median, dispersion и 95% CI (Confidence Interval — доверительный интервал). Для success/collision fractions использовать Wilson interval; для парных differences — paired bootstrap по episode ID (10 000 resamples, фиксированный analysis seed). Per-frame metrics сначала агрегировать в episode, чтобы не раздувать sample size. Для G4/G5 training seeds показывать отдельно; нельзя считать 3 × 20 независимыми training runs. При hierarchical bootstrap описать уровни и ограничения оценки по трём seeds.

Парные сравнения C1…C6 с C0 используют одинаковые scenario IDs/initial states и порядок прогонов, перемешанный независимо от метода. CI (Confidence Interval — доверительный интервал) описывает именно эту тестовую выборку. Для p-values при множественных сравнениях заранее выбрать correction (например Holm) или ограничиться effect sizes + CI (Confidence Interval — доверительный интервал) без утверждений о statistical significance.

Обязательны: learning/validation curves, таблица per-scenario metrics, минимум одна ablation на группу, 3 failure cases на группу, анализ runtime cost и один случай propagation ошибки между компонентами. Улучшение reward не заменяет delivery/safety metrics. Недостающие логи — missing data с объяснением, не zero; crash/timeout входят в результаты.

## Формат результатов

Студенты реализуют C++ runner и C++ aggregator. Runner выполняет reset/seed/configure, ждёт readiness, запускает mission, завершает по terminal reason, пишет metrics и manifest. Aggregator проверяет полную матрицу run IDs, дубликаты и pairing, создаёт CSV (Comma-Separated Values — табличный текстовый формат со значениями, разделёнными запятыми) summaries и confidence intervals. Иллюстрации разрешено делать табличным редактором или другим plotting tool из готовых CSV; первичные метрики считаются C++.

Минимальная CSV schema:

```text
episode_id,config_id,scenario_id,layout_id,scenario_seed,training_seed_bundle,terminal_reason,elapsed_sim_s,delivered,released,on_time,collision_count,distance_m,min_clearance_m,deadline_misses,safety_interventions,fallback_count
```

Детальные component latency/ATE/IoU (Intersection over Union — отношение площади пересечения к площади объединения) и job events — отдельные long-format CSV с episode ID. Manifest: commit SHA (Secure Hash Algorithm — семейство алгоритмов хеширования; здесь обозначение хеша коммита), dirty=false, Docker image digest/ID, dependency versions, world/map/data hashes, model SHA256 (Secure Hash Algorithm, 256-bit — алгоритм хеширования с результатом длиной 256 бит), hardware, thread limits, all seeds, config, start/end times. Использовать [шаблон manifest](../templates/experiment_manifest.json). Все `TO_FILL` должны быть заменены перед оцениваемым запуском.

Поставить `results/<release-tag>/` с raw CSV, aggregate CSV, figures, reproduction commands, links/checksums на bags и models. Большие артефакты — release assets или внешнее хранилище с доступом преподавателю; Git содержит manifests, инструкции и малый replay fixture. Не коммитить гигабайты bags/checkpoints.
