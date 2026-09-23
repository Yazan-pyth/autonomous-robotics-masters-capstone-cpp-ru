# Работа команды, Git и календарь

## Роли 12 студентов

| Студент | Группа | Основная зона | Интеграционная обязанность |
|---|---|---|---|
| S01 | G1 | EKF/ICP/TF | State contract с G4 |
| S02 | G1 | Learned covariance/data | Model metadata и replay с G3 |
| S03 | G1 | Simulator/robot/sensors | Bridge, clocks, reset с G5 |
| S04 | G2 | Geometric perception | RGB-D/TF contract с G1 |
| S05 | G2 | CNN/inference | Obstacle grid contract с G3/G4 |
| S06 | G3 | A*/action | Path/cancel contract с G4/G5 |
| S07 | G3 | Learned cost | Replan scenario и validation с G2 |
| S08 | G4 | MPC | FollowPath и failure feedback с G5 |
| S09 | G4 | PPO | Model loading и CPU budget с G1/G2 |
| S10 | G4 | Safety/faults | Driver watchdog с G1 |
| S11 | G5 | BT/dispatch | Общий bringup/CI с владельцами пакетов |
| S12 | G5 | DQN/benchmark | Manifest/metrics API со всеми группами |

S01…S12 — placeholders; имена студенты подставляют при старте. Роли не освобождают от понимания второго метода. Integration lead ротируется по группам раз в две недели; один студент не должен становиться единственным человеком, который умеет запускать систему.

## Milestones на 14 недель

| Неделя | Результат и критерий готовности |
|---|---|
| 1 | Команды, research questions, literature review, clone/build starter, учёт доступных CPU/GPU |
| 2 | Freeze интерфейсов v1, simulator choice, sensors/limits, split и scenario manifests, budget на обучение |
| 3–4 | Robot/sensors + все classical stubs заменены рабочими методами; первая C0 доставка; CI и replay |
| 5–6 | Устойчивый C0, train data, correctness tests, baseline metrics; никаких ожиданий готовности чужой модели |
| 7–8 | Все пять learned variants работают из C++; model cards и первые isolated comparisons |
| 9–10 | C1…C6, fault tests, полный reset, profiler; freeze dataset/model selection по validation |
| 11 | Code/config freeze, final benchmark rehearsal, capacity check; smoke не считается итоговым экспериментом |
| 12 | Final 560 system runs, isolated comparisons и training-seed evaluation; только documented reruns |
| 13 | Analysis, CI/effect sizes, failure cases, five reports + integration draft, воспроизведение другой группой |
| 14 | Release, демонстрация C0/C6, fault demo и индивидуальная защита |

При 20 часах проекта в неделю на подгруппу суммарная capacity порядка 1400 group-hours; конкретное расписание преподаватель адаптирует до старта. Время GPU и длительность прогонов резервируются заранее. Дополнительные архитектуры/симуляторы допустимы только после выполнения core requirements.

## Git-процесс

- Один общий репозиторий с пакетами `capstone_localization`, `capstone_perception`, `capstone_planning`, `capstone_control`, `capstone_safety`, `capstone_mission`, `capstone_benchmark`. В starter существует только `capstone_bringup`; остальные пакеты создают группы.
- Branch `gN/issue-description`, короткие PR в `main`. Один review внутри группы; для интерфейса — review группы-потребителя. В PR: задача, изменения, проверки, reproduction command и связанные issues.
- Issues имеют group, milestone и owner. Чисто документальная правка не требует симуляции, но изменение интерфейса требует contract test.
- Не коммитить bags/checkpoints/secrets/build products. Малые fixtures — с происхождением и лицензией; модели — release assets с SHA256.
- Теги: `v0.1-contracts`, `v0.2-classical`, `v0.3-learned`, `v1.0-submission`. Starter публикуется как `v0.1.0-assignment`, чтобы не путать его с результатами студентов.
- Защитить `main` и включить required CI/review в настройках при запуске учебной команды. Эти настройки должен включить владелец; наличие текста здесь не означает, что branch protection уже настроен.

## Совместная разработка без блокировок

Каждая группа предоставляет небольшой bag/input fixture и reference outputs. До live integration consumers работают с replay. G1 подготавливает simulator adapter, G4 — safety, G5 — bringup/runner; остальные поставляют свои launch/config и зависимости. На integration meeting показывать один воспроизводимый дефект и его regression check, а не только slides.

Изменение контракта: issue → запись причины/альтернатив/миграции в `docs/adr/` → review producer+consumer → обновление tests → merge. Новый learned method не должен ломать classical path.
