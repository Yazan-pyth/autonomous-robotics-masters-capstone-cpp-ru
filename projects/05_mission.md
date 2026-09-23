# G5 — Миссии, диспетчеризация и общая сборка

**2 студента.** Исследовательский вопрос: улучшает ли DQN выбор заявок относительно фиксированного dispatch rule при задержках и ограниченных сроках?

## Два метода

**A — классический:** Behavior Tree управляет `wait → select job → navigate pickup → service → navigate dropoff → service/recover`; выбор — earliest due date, tie-break по оценённой длине маршрута, затем job ID.

**B — RL:** тот же BT, но DQN выбирает следующую заявку из максимум четырёх pending jobs или `wait`. Observation: оценки travel time, slack, queue age, robot/load state, последние navigation failures. Невалидные действия маскируются одинаково при training и inference; при неизвестном ID/NaN — fallback A. Новые заявки поступают по одному расписанию для A/B, без доступа к будущим arrival events.

DQN не управляет мотором, не меняет пути напрямую и не учится заново во время test. Выбор делается только когда робот свободен; активную доставку нельзя бросить ради удобной статистики. Повторная навигация и recovery остаются в общем BT.

При очереди длиннее четырёх заявок обеим политикам выдаются четыре старейшие по `(release_time, job_id)`; остальные сохраняются в очереди и учитываются в итоговых метриках. Slots 0–3 соответствуют этому стабильному порядку, action 4 — `wait` на 1 с simulation time. После ожидания или завершения доставки candidate set обновляется; невалидные slots маскируются. Классический метод выбирает earliest due date из того же candidate set. Нельзя удалять неудобные заявки или использовать будущие arrivals.

## Задачи

1. Формализовать job schema, released/picked/delivered/failed states и временные ограничения.
2. Реализовать BT, action calls к G3/G4, preemption, feedback, timeout и bounded recovery (максимум 2 попытки на этап).
3. Сделать дешёвую offline dispatch-среду на train travel-time tables; проверить domain gap при переносе DQN в full simulation.
4. Обучить DQN минимум с тремя training seeds, экспортировать и внедрить C++ inference.
5. Координировать bringup/config profiles, scenario reset, manifests и общий benchmark runner на C++.
6. Собрать integration report из результатов всех групп; остальные группы сами поставляют свои данные и разделы.

## Вход / выход

Файл manifest содержит job list и release times; runtime раскрывает только наступившие события. Узел G5 вызывает `/planning/compute_path` и `/control/follow_path`, журналирует переходы. `/mission/status` (`DiagnosticArray`) для мониторинга; структурированный event log содержит episode ID, job ID, sim time, event/reason, target.

## Эксперименты и метрики

Completed deliveries per simulated hour, on-time delivery rate среди всех released jobs, mean/p95 tardiness (для завершённых) + отдельно unfinished jobs и их age. Дополнительно makespan для finite-job suite, traveled distance per completed job, idle time, failed jobs, recoveries и decision latency. Если completed=0, distance/job — NA, а не ноль.

Тесты: низкая/высокая интенсивность заявок, узкое место, blocked route, неравные deadlines. Ablation: DQN без congestion feature. Isolated dispatch тестировать на фиксированных train-independent travel tables; closed-loop — с одинаковыми G1–G4. Не выдавать выигрыш на табличной модели за выигрыш полной системы.

## Приёмка и артефакты

Пакеты `capstone_mission`, `capstone_benchmark`, bringup configs, BT XML, DQN artifacts, ledger и report. Тесты: empty queue, masked actions, pickup/dropoff order, action cancel, retry limit, failure to stop. Роли: студент 1 — BT/interfaces; студент 2 — DQN/evaluation; координация CI и runner совместная. G5 не несёт единоличную ответственность за ошибки остальных групп.
