# G1 — Локализация, геометрическая карта и симуляционные датчики

**3 студента.** Исследовательский вопрос: помогает ли адаптивная оценка шума сохранять точность и consistency при wheel slip и вырожденном scan matching?

## Два метода

**A — классический:** EKF в SE(2), predict по wheel odometry/IMU, correction по scan-to-map ICP; ковариация ICP фиксируется на validation. Outlier gating и bounds одинаковы у обоих методов.

**B — DL:** тот же фильтр и ICP, но небольшая MLP прогнозирует положительную диагональ ковариации ICP по доступным residual/fitness, числу inliers, геометрии scan и истории innovation. Использовать softplus + floor/ceiling; не позволять сети выдавать некорректную матрицу. Фиксированный EKF остаётся fallback. Ground truth используется только для train labels и evaluator.

## Задачи

1. Подготовить модель differential drive, wheel odometry, IMU, LiDAR и RGB-D с G2; параметры сенсоров и TF сохранять в Git.
2. Получить общую occupancy map классическим SLAM, документировать loop closure и map artifacts.
3. Реализовать fusion, синхронизацию timestamps, gating и публикацию covariance. Обосновать motion/measurement models.
4. Сформировать train/validation logs без пересечения траекторий и миров с test; обучить MLP и экспортировать её.
5. Реализовать C++ preprocessing/inference, validate-model test, контроль NaN и fallback.
6. Передать G2/G3/G4 общий TF tree и записанные bags для независимой разработки.

## Вход / выход

Входы: `/sensors/wheel_odom`, `/sensors/imu`, `/sensors/scan`, `/map`. Выходы: `/state/odom`, `/localization/pose`, `/tf`, diagnostics. Только G1 публикует `map → odom` и `odom → base_link`; sensor transforms задаются статически. См. [контракты](../docs/INTERFACES.md).

## Эксперименты и метрики

- ATE RMSE в общей карте без post-hoc alignment, RPE translation/yaw для интервала 1 с; p95 ошибки положения.
- NEES для `(x,y,yaw)` при доступной GT, NIS для innovation; доля статистически несогласованных оценок и диагностика причин.
- Доля потерь локализации: ошибка >0.5 м или >15° непрерывно >1 с; время восстановления.
- Callback/inference p50/p95/p99, missed deadlines, fallback fraction.
- Map IoU и время построения карты — отдельно от сравнения online localization.
- Сценарии: нормальная сцена, slip, бедный геометрией проход, dropout IMU/LiDAR. Ablation: MLP без истории innovation.

## Приёмка и артефакты

Оба варианта работают на одном bag, одинаково обрабатывают reset `/clock`, не используют `/evaluation/*`, сохраняют valid covariance. Проверки: поворот ±π, stale IMU, ICP outlier, NaN модели. Поставить пакет `capstone_localization`, robot/world assets, frozen map и metadata, training code, модель, CSV/plots и отчёт. SLAM можно взять из библиотеки, но EKF/адаптация и их анализ должны быть работой группы.

Роли: студент 1 — fusion/geometry; студент 2 — MLP/data; студент 3 — simulator/sensors, replay и интеграция. Каждый review-ит другую часть и объясняет оба метода на защите.
