# Шаблон отчёта

Группа / авторы / release tag / commit / дата / ссылки на artifacts.

Для подпроекта 8–12 страниц, для integration report 10–15, без приложений.

1. Задача, research question, primary metric и гипотеза до test.
2. Related work и связь с модулями курса; собственный вклад и использованные библиотеки.
3. Математическая постановка A/B: assumptions, inputs, constraints, losses/reward, complexity.
4. Реализация: C++ architecture, ROS contracts, dependencies, inference и fallback.
5. Данные и обучение: split по сценам/эпизодам, seeds, budget, checkpoint selection, model card.
6. Протокол: одинаковые условия, hardware, manifests, сценарии, terminal reasons, анализ missing runs.
7. Результаты: raw/aggregate links, per-scenario metrics, CI/effect sizes, learning curves, runtime/memory.
8. Ablation и минимум 3 failure cases с evidence, включая отсутствие улучшения.
9. Интеграция: downstream impact, contracts, fault tests, propagation ошибок.
10. Ограничения, выводы и воспроизведение из clean clone точными командами.
11. Индивидуальные роли со ссылками на contribution records.

| Scenario | Method | n episodes / training seeds | Primary metric + 95% CI | Safety | p95 latency | Fallback fraction |
|---|---|---|---|---|---|---|
| заполнить | classical | | | | | |
| заполнить | learned | | | | | |

Integration report дополнительно содержит C0…C6, 560-run completeness, software diagram, job ledger, Docker/image versions и общую таблицу 40-балльной самооценки с evidence. Не вставлять вымышленные «примерные» результаты в таблицы фактических экспериментов.
