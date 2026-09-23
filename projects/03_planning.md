# G3 — Глобальное планирование в меняющейся среде

Сокращения раскрыты при первом употреблении; [полный словарь терминов](../docs/GLOSSARY.md).

**2 студента.** Исследовательский вопрос: помогает ли обученная стоимость проходимости выбирать маршруты с меньшими задержками, сохраняя ограничения на столкновения?

## Два метода

**A — классический:** A* (A-star — алгоритм поиска пути с оценкой уже пройденной стоимости и эвристикой оставшегося пути) на 8-связной сетке с фиксированной стоимостью длины, clearance и наблюдаемой occupancy. Не допускать corner cutting. Эвристика octile distance умножается на нижнюю границу стоимости шага.

**B — DL (Deep Learning — глубокое обучение):** A* с той же реализацией поиска и hard collision mask, но CNN (Convolutional Neural Network — свёрточная нейронная сеть) прогнозирует дополнительную неотрицательную стоимость задержки/риска по local occupancy history, static map и доступному состоянию. Модель не получает future obstacle trajectories. Вне наблюдаемой области — фиксированный prior. Нижняя граница стоимости шага сохраняется; admissible heuristic проверяется для новой функции.

Это исследование learned cost, а не обещание, что neural A* оптимален по физическому времени. Внутренняя cost разных методов различается; окончательные сравнения выполняются по общей внешней метрике, определённой до test.

## Задачи

1. Реализовать map fusion, transform локальной сетки в `map`, inflation с общей геометрией робота.
2. Реализовать A*, cancellation, blocked-goal/no-path behavior и планирование при обновлении карты.
3. Получить обучающие labels задержки из train rollouts фиксированного классического контроллера, документировать bias этих labels.
4. Обучить компактную CNN, внедрить C++ inference и fallback на фиксированную cost.
5. Подключить стандартный `ComputePathToPose` action и обеспечить совместимость с обоими контроллерами G4.

## Вход / выход

`/map`, `/perception/obstacles`, `/localization/pose` и goal → action `/planning/compute_path` (`nav2_msgs/action/ComputePathToPose`), diagnostic status. Успешный action result содержит `nav_msgs/Path` в `map`; пустой/no-path результат должен быть явной ошибкой, а не успешным маршрутом. G5 передаёт path в `FollowPath` G4.

## Эксперименты и метрики

Planning success, planning latency p50 (50th percentile — 50-й процентиль, медиана)/p95 (95th percentile — 95-й процентиль)/p99 (99th percentile — 99-й процентиль), deadline misses, expanded nodes, длина пути, min/5th percentile clearance, replans per mission. В closed-loop с одним фиксированным G4: collision/success, travel time, delay и completed deliveries. No-path и timeouts включать в знаменатель; path length показывать с условием успешного решения.

Сценарии: узкий проход, заблокированный коридор, два пути разной длины/загруженности. Ablation: learned cost без истории occupancy. Для малых карт сравнить A* с Dijkstra по **той же** cost как correctness test; не сравнивать стоимость A и B как единую quality metric.

## Приёмка и артефакты

Пакет `capstone_planning`, обе cost-модели, map/goal tests (включая unreachable), model/data cards, replay input, metrics и отчёт. Hard collision mask одинаков для обоих методов; confidence модели не разрешает проход через занятые клетки. Роли: студент 1 — поиск/contracts; студент 2 — data/model; совместно profiling и integration review с G2/G4.
