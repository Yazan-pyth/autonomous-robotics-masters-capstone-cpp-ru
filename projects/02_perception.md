# G2 — Восприятие препятствий и локальная проходимость

**2 студента.** Исследовательский вопрос: насколько CNN улучшает обнаружение непроходимых областей при шуме глубины, изменении освещения и новых формах препятствий?

## Два метода

**A — классический:** из RGB-D восстановить cloud, выделить плоскость пола (RANSAC), отфильтровать точки по высоте и объединить obstacle clusters; проектировать их в локальную сетку. Цвет не обязателен для эвристики, но доступен обоим методам.

**B — DL:** компактная RGB-D CNN с классами `free / obstacle / unknown`, затем та же геометрическая проекция в grid. Invalid depth остаётся unknown; сеть не имеет права объявить область с отсутствующей геометрией безопасной только по цвету. Выходные интерфейсы и postprocessing согласованы.

## Задачи

1. Согласовать intrinsics, depth units, CameraInfo, extrinsics и synchronizer с G1.
2. Реализовать оба pipelines и одинаковые правила grid projection/inflation boundaries.
3. Сгенерировать/разметить train/validation данные; split делать по сценам и эпизодам, не соседним кадрам.
4. Обучить CNN, экспортировать, реализовать C++ preprocessing и inference, проверить golden tensors.
5. Выдавать `OccupancyGrid` в локальном окне, возраст данных и diagnostic status.
6. Подготовить replay-набор для G3/G4 и тест missing depth / stale frame.

## Вход / выход

Входы: RGB, depth, CameraInfo, TF. Выход: `/perception/obstacles` с occupancy 0–100 и unknown −1; frame `odom`. Номинально окно 10 × 10 м, resolution 0.05 м, publication 10 Гц; origin обязан соответствовать координатам сетки, а не индексу робота.

## Эксперименты и метрики

IoU каждого класса и macro-IoU, obstacle precision/recall, false-free rate среди GT obstacle cells, latency p50/p95/p99 и peak RSS. Оценка в общей valid ROI; unknown/occluded GT обрабатывается отдельно. Сравнивать perception metrics на одинаковых replay frames; влияние на collision/success — в интеграции. Сценарии: normal, low light, thin obstacles, depth holes; ablation без RGB либо без augmentation.

Отдельно привести confusion matrix, ошибки по расстоянию и не менее трёх failure cases. Независимый LiDAR safety monitor не является частью качества CNN и не должен скрывать её false-free ошибки.

## Приёмка и артефакты

Пакет `capstone_perception`, генератор/описание датасета, классический pipeline, обученная модель, тесты invalid depth и frame mismatch, model card, CSV и отчёт. Во время live run нет simulator segmentation labels на входе узла; они доступны только в обучении/evaluator. Роли: студент 1 — geometry/ROS/replay; студент 2 — CNN/export/calibration; взаимное review и совместный analysis.
