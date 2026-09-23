# Первичные источники и технические основания

Сокращения раскрыты при первом употреблении; [полный словарь терминов](GLOSSARY.md).

Стек выбран для соответствия исходному курсу. Документацию API (Application Programming Interface — программный интерфейс) читать для Jazzy/Harmonic, а не автоматически переносить примеры Rolling. Версии библиотек студенты фиксируют в образах.

- [Исходный курс](https://github.com/Yazan-pyth/advanced_autonomous_mobile_robots_cpp_ru) и [правила C++ deployment](https://github.com/Yazan-pyth/advanced_autonomous_mobile_robots_cpp_ru/blob/main/docs/CPP_ML_DEPLOYMENT.md): C++ runtime, offline training и model verification.
- [ROS (Robot Operating System — программная платформа для робототехники)–Gazebo repository/version mapping](https://index.ros.org/r/ros_gz/): связка Jazzy/Harmonic.
- [ros_gz](https://docs.ros.org/en/jazzy/p/ros_gz/): интеграционные пакеты ROS/Gazebo.
- [Gazebo Harmonic: SDF (Simulation Description Format — формат описания симуляционных миров и моделей) worlds](https://gazebosim.org/docs/harmonic/sdf_worlds/): physics system и описание мира.
- [ros_gz_bridge, ветка jazzy](https://github.com/gazebosim/ros_gz/tree/jazzy/ros_gz_bridge): bridge, направление сообщений и `/clock`.
- [Nav2 (Navigation 2 — стек навигации для второй версии платформы Robot Operating System) Collision Monitor, Jazzy](https://docs.nav2.org/jazzy/configuration_and_development/configuration_guide/core_servers/collision_monitor/): архитектурный пример независимого защитного уровня; проект требует собственного обоснования safety envelope.
- [ONNX (Open Neural Network Exchange — открытый формат обмена моделями нейронных сетей) Runtime C++](https://onnxruntime.ai/docs/get-started/with-cpp.html): C++ inference API.
- [PPO (Proximal Policy Optimization — алгоритм оптимизации политики с ограничением величины её обновления), авторская статья](https://arxiv.org/abs/1707.06347): алгоритм для G4.
- [DQN (Deep Q-Network — глубокая нейронная сеть для оценки ценности действий), авторская статья](https://www.nature.com/articles/nature14236): алгоритм для G5.

Эти источники обосновывают инструменты и алгоритмы, а распределение команд, сценарии, лимиты, grading и объём экспериментов являются требованиями данного учебного задания. Значения latency и safety margins не являются гарантией библиотек или сертификатом безопасности.
