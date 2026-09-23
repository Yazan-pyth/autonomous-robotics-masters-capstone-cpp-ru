# G2 — Восприятие препятствий и локальная проходимость

Сокращения раскрыты при первом употреблении; [полный словарь терминов](../docs/GLOSSARY.md).

Исследовательский вопрос: насколько CNN (Convolutional Neural Network — свёрточная нейронная сеть) улучшает обнаружение непроходимых областей при шуме глубины, изменении освещения и новых формах препятствий?

## Два метода

**A — классический:** из RGB-D (Red, Green, Blue and Depth — цветное изображение и карта глубины) восстановить cloud, выделить плоскость пола (RANSAC (Random Sample Consensus — оценивание модели по согласованности случайных выборок)), отфильтровать точки по высоте и объединить obstacle clusters; проектировать их в локальную сетку. Цвет не обязателен для эвристики, но доступен обоим методам.

**B — DL (Deep Learning — глубокое обучение):** компактная RGB-D CNN с классами `free / obstacle / unknown`, затем та же геометрическая проекция в grid. Invalid depth остаётся unknown; сеть не имеет права объявить область с отсутствующей геометрией безопасной только по цвету. Выходные интерфейсы и postprocessing согласованы.

## Задачи

1. Согласовать intrinsics, depth units, CameraInfo, extrinsics и synchronizer с G1.
2. Реализовать оба pipelines и одинаковые правила grid projection/inflation boundaries.
3. Сгенерировать/разметить train/validation данные; split делать по сценам и эпизодам, не соседним кадрам.
4. Обучить CNN, экспортировать, реализовать C++ preprocessing и inference, проверить golden tensors.
5. Выдавать `OccupancyGrid` в локальном окне, возраст данных и diagnostic status.
6. Подготовить replay-набор для G3/G4 и тест missing depth / stale frame.

## Вход / выход

Входы: RGB (Red, Green, Blue — красный, зелёный и синий цветовые каналы), depth, CameraInfo, TF (Transform library — библиотека преобразований между системами координат). Выход: `/perception/obstacles` с occupancy 0–100 и unknown −1; frame `odom`. Номинально окно 10 × 10 м, resolution 0.05 м, publication 10 Гц; origin обязан соответствовать координатам сетки, а не индексу робота.

## Эксперименты и метрики

IoU (Intersection over Union — отношение площади пересечения к площади объединения) каждого класса и macro-IoU, obstacle precision/recall, false-free rate среди GT (Ground Truth — эталонные данные для обучения или оценки) obstacle cells, latency p50 (50th percentile — 50-й процентиль, медиана)/p95 (95th percentile — 95-й процентиль)/p99 (99th percentile — 99-й процентиль) и peak RSS (Resident Set Size — объём физической памяти, занятой процессом). Оценка в общей valid ROI (Region of Interest — область интереса, в которой проводится оценка); unknown/occluded GT обрабатывается отдельно. Сравнивать perception metrics на одинаковых replay frames; влияние на collision/success — в интеграции. Сценарии: normal, low light, thin obstacles, depth holes; ablation без RGB либо без augmentation.

Отдельно привести confusion matrix, ошибки по расстоянию и не менее трёх failure cases. Независимый LiDAR (Light Detection and Ranging — измерение расстояний с помощью света; лазерный дальномер) safety monitor не является частью качества CNN и не должен скрывать её false-free ошибки.

## Приёмка и артефакты

Пакет `capstone_perception`, генератор/описание датасета, классический pipeline, обученная модель, тесты invalid depth и frame mismatch, model card, CSV (Comma-Separated Values — табличный текстовый формат со значениями, разделёнными запятыми) и отчёт. Во время live run нет simulator segmentation labels на входе узла; они доступны только в обучении/evaluator. Обязанности по реализации, проверке и интеграции распределяются внутри группы.
