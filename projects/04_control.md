# G4 — Локальное управление, PPO и независимая безопасность

**3 студента.** Исследовательский вопрос: как PPO и MPC отличаются по tracking, избеганию препятствий и устойчивости к model mismatch при одинаковом safety monitor?

## Два метода

**A — классический:** receding-horizon MPC для differential-drive модели; tracking/path progress, effort, constraints на скорость/ускорение и препятствия. Обосновать horizon, weights, discretization и поведение при infeasible solve. Готовый solver допустим, собственные formulation и tests обязательны.

**B — RL:** PPO выдаёт `(v, ω)` по robot-frame path segment, оценке состояния и local obstacle grid/ranges. Observation и action specification, normalization, reward, terminal conditions и training budget фиксируются в Git. Оба метода имеют доступ к одинаковой информации; PPO не получает GT pose или будущие препятствия.

Независимый deterministic safety monitor одинаков у A и B. Он проверяет freshness, пределы, NaN, stopping distance и ближайшие препятствия по raw LiDAR. Он единственный публикует `/cmd_vel`. Он не заменяет корректность контроллера.

## Задачи

1. Реализовать `FollowPath` action: preemption, cancel, success/failure, progress checker.
2. Реализовать MPC с solver timeout и измерением end-to-end cycle latency.
3. Обучить PPO минимум с тремя training seeds; policy selection только по validation; внедрить C++ inference.
4. Реализовать monitor: halt на stale input, nonfinite command и риске столкновения; clamping не считается достаточным collision avoidance.
5. Создать fault suite: sensor silence, delayed command, invalid model output, infeasible optimization, узкий проход.
6. Передать G5 navigation status и runner hooks; обеспечить stop на отмене action.

## Вход / выход

`/control/follow_path` (`nav2_msgs/action/FollowPath`), `/state/odom`, TF, `/perception/obstacles`, scan → `/control/cmd_vel_raw` (`TwistStamped`); monitor → `/cmd_vel` (`TwistStamped`). Simulator adapter снимает stamp только если его driver требует unstamped Twist. Оба controller modes работают 20 Гц.

## Эксперименты и метрики

Cross-track RMSE/p95, yaw error, success/collision/timeout, min clearance, время, variation управляющего сигнала `sum ||u[t]-u[t-1]||`, constraint violation counts, p95/p99 cycle latency, missed deadlines. Показать intervention rate monitor, причины fallback и результат каждого training seed. Reward curve — дополнительный training diagnostic.

Тесты: прямой/кривой путь, динамическое препятствие, payload/model mismatch ±20%, задержки. Ablation: PPO без domain randomization. Прогоны без shield допускаются только в симуляторе и маркируются отдельно; они не входят в основной безопасный deployment. Отключение shield не является обязательным.

## Приёмка и артефакты

Пакеты `capstone_control`, `capstone_safety`; formulation MPC, PPO training code/model, safety thresholds, fault logs, C++ tests, metrics и отчёт. При silent publisher downstream watchdog останавливает robot; один последний нулевой Twist недостаточен. Роли: студент 1 — MPC; студент 2 — PPO; студент 3 — monitor/faults. Каждый участвует в общей интеграции.
