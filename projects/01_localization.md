# G1 — Локализация, геометрическая карта и симуляционные датчики

Сокращения раскрыты при первом употреблении; [полный словарь терминов](../docs/GLOSSARY.md).

**3 студента.** Исследовательский вопрос: помогает ли адаптивная оценка шума сохранять точность и consistency при wheel slip и вырожденном scan matching?

## Два метода

**A — классический:** EKF (Extended Kalman Filter — расширенный фильтр Калмана) в SE(2) (Special Euclidean Group in Two Dimensions — группа перемещений и поворотов твёрдого тела на плоскости), predict по wheel odometry/IMU (Inertial Measurement Unit — инерциальный измерительный модуль), correction по scan-to-map ICP (Iterative Closest Point — итеративный алгоритм ближайших точек для совмещения облаков точек); ковариация ICP фиксируется на validation. Outlier gating и bounds одинаковы у обоих методов.

**B — DL (Deep Learning — глубокое обучение):** тот же фильтр и ICP, но небольшая MLP (Multilayer Perceptron — многослойный перцептрон) прогнозирует положительную диагональ ковариации ICP по доступным residual/fitness, числу inliers, геометрии scan и истории innovation. Использовать softplus + floor/ceiling; не позволять сети выдавать некорректную матрицу. Фиксированный EKF остаётся fallback. Ground truth используется только для train labels и evaluator.

## Задачи

1. Подготовить модель differential drive, wheel odometry, IMU, LiDAR (Light Detection and Ranging — измерение расстояний с помощью света; лазерный дальномер) и RGB-D (Red, Green, Blue and Depth — цветное изображение и карта глубины) с G2; параметры сенсоров и TF (Transform library — библиотека преобразований между системами координат) сохранять в Git.
2. Получить общую occupancy map классическим SLAM (Simultaneous Localization and Mapping — одновременная локализация и построение карты), документировать loop closure и map artifacts.
3. Реализовать fusion, синхронизацию timestamps, gating и публикацию covariance. Обосновать motion/measurement models.
4. Сформировать train/validation logs без пересечения траекторий и миров с test; обучить MLP и экспортировать её.
5. Реализовать C++ preprocessing/inference, validate-model test, контроль NaN (Not a Number — специальное значение «не число») и fallback.
6. Передать G2/G3/G4 общий TF tree и записанные bags для независимой разработки.

## Вход / выход

Входы: `/sensors/wheel_odom`, `/sensors/imu`, `/sensors/scan`, `/map`. Выходы: `/state/odom`, `/localization/pose`, `/tf`, diagnostics. Только G1 публикует `map → odom` и `odom → base_link`; sensor transforms задаются статически. См. [контракты](../docs/INTERFACES.md).

## Эксперименты и метрики

- ATE (Absolute Trajectory Error — абсолютная ошибка траектории) RMSE (Root Mean Square Error — среднеквадратическая ошибка) в общей карте без post-hoc alignment, RPE (Relative Pose Error — относительная ошибка положения и ориентации) translation/yaw для интервала 1 с; p95 (95th percentile — 95-й процентиль) ошибки положения.
- NEES (Normalized Estimation Error Squared — нормированный квадрат ошибки оценивания) для `(x,y,yaw)` при доступной GT (Ground Truth — эталонные данные для обучения или оценки), NIS (Normalized Innovation Squared — нормированный квадрат невязки измерения) для innovation; доля статистически несогласованных оценок и диагностика причин.
- Доля потерь локализации: ошибка >0.5 м или >15° непрерывно >1 с; время восстановления.
- Callback/inference p50 (50th percentile — 50-й процентиль, медиана)/p95/p99 (99th percentile — 99-й процентиль), missed deadlines, fallback fraction.
- Map IoU (Intersection over Union — отношение площади пересечения к площади объединения) и время построения карты — отдельно от сравнения online localization.
- Сценарии: нормальная сцена, slip, бедный геометрией проход, dropout IMU/LiDAR. Ablation: MLP без истории innovation.

## Приёмка и артефакты

Оба варианта работают на одном bag, одинаково обрабатывают reset `/clock`, не используют `/evaluation/*`, сохраняют valid covariance. Проверки: поворот ±π, stale IMU, ICP outlier, NaN модели. Поставить пакет `capstone_localization`, robot/world assets, frozen map и metadata, training code, модель, CSV (Comma-Separated Values — табличный текстовый формат со значениями, разделёнными запятыми)/plots и отчёт. SLAM можно взять из библиотеки, но EKF/адаптация и их анализ должны быть работой группы.

Роли: студент 1 — fusion/geometry; студент 2 — MLP/data; студент 3 — simulator/sensors, replay и интеграция. Каждый review-ит другую часть и объясняет оба метода на защите.
