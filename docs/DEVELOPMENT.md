# Разработка и Docker

Сокращения раскрыты при первом употреблении; [полный словарь терминов](GLOSSARY.md).

## Что уже существует

- Два Docker build targets: `simulator` (Gazebo + bridge) и `autonomy` (C++ ROS (Robot Operating System — программная платформа для робототехники) package).
- Пустой SDF (Simulation Description Format — формат описания симуляционных миров и моделей)-мир с physics. Он не содержит робота, склада, датчиков или ground truth adapter.
- `capstone_bringup/clock_probe`: C++ подписчик, проверяющий несколько строго возрастающих значений `/clock` и завершающийся с ненулевым кодом при wall-clock timeout.
- Unit test на frozen/regressing/invalid clock и headless Compose smoke в GitHub Actions.

Это инфраструктурная проверка DDS (Data Distribution Service — стандарт обмена данными распределённых систем) между двумя контейнерами. Студенты реализуют robot/world assets, navigation, datasets/models и final benchmark. CI (Continuous Integration — непрерывная интеграция; автоматическая сборка и проверки) starter не проверяет качество несуществующих алгоритмов.

## Команды

Требуются Docker Engine + Compose v2 либо Docker Desktop с Linux containers. Первый build скачивает ROS/Gazebo packages; GPU (Graphics Processing Unit — графический процессор) и GUI (Graphical User Interface — графический интерфейс пользователя) для smoke не нужны.

```bash
docker compose build
docker compose up --abort-on-container-exit --exit-code-from autonomy
docker compose logs --no-color
docker compose down
```

Ожидается `PASS: simulation clock advanced` и exit code 0 контейнера `autonomy`. Отдельно повторить тесты образа:

```bash
docker compose run --rm --no-deps autonomy bash -lc 'cd /ws && source /opt/ros/jazzy/setup.bash && colcon test --event-handlers console_direct+ && colcon test-result --verbose'
```

Для исходников внутри dev container используйте отдельный Compose override с bind mount рабочей копии и командой shell; существующий production-like smoke копирует код в image. После изменения исходников нужна повторная сборка image.

Локальная ROS 2 (Robot Operating System 2 — вторая версия программной платформы для робототехники) среда на Ubuntu:

```bash
source /opt/ros/jazzy/setup.bash
cd ros2_ws
colcon build --cmake-args -DCMAKE_BUILD_TYPE=RelWithDebInfo
colcon test --event-handlers console_direct+
colcon test-result --verbose
```

`clock_probe` сам по себе завершится ошибкой без живого `/clock`; это ожидаемая проверка, не standalone navigation demo.

## Контейнерная архитектура

Compose использует общую bridge network, один `ROS_DOMAIN_ID=42`, middleware CycloneDDS и UDP (User Datagram Protocol — протокол передачи пользовательских дейтаграмм) discovery. `ROS_LOCALHOST_ONLY=0`; host networking, privileged mode и host Docker socket не нужны. Simulator clock bridge внутри simulator container, C++ probe внутри autonomy. Если локальный Docker/network фильтрует multicast, зафиксировать CycloneDDS peer configuration и проверить её на обеих машинах; не считать успешный native запуск доказательством двухконтейнерной связи.

Build base `ros:jazzy-ros-base` обновляется upstream. Starter не обещает bit-for-bit repeatability этого тега. Для итоговой сдачи сохранить image IDs (Identifiers — идентификаторы)/digests, список пакетов и base digest; использовать один замороженный образ для всей серии. ONNX (Open Neural Network Exchange — открытый формат обмена моделями нейронных сетей) Runtime/LibTorch и solver пока не установлены — каждая группа добавляет совместимые фиксированные версии и license metadata.

В `docker/start_simulator.sh` shutdown trap останавливает bridge и Gazebo, а ошибка любого дочернего процесса завершает контейнер. В `autonomy` probe имеет 45-секундный steady-clock deadline, чтобы отсутствие `/clock` не приводило к зависанию CI (Continuous Integration — непрерывная интеграция; автоматическая сборка и проверки). При создании реального bringup заменить smoke CMD (Command — инструкция Dockerfile, задающая команду контейнера по умолчанию) на запуск всех компонентов, сохранив smoke как отдельный тест.

## Требования к финальным контейнерам

1. Clean clone → documented build → один запуск mission без ручной правки файлов контейнера.
2. Headless режим обязателен; GUI/RViz (ROS Visualization — средство визуализации данных робототехнической платформы) — optional profile. CPU (Central Processing Unit — центральный процессор) mode обязателен; GPU compose override отдельный.
3. Sensor/command bridges, TF (Transform library — библиотека преобразований между системами координат), `/clock`, seed/reset и readiness тестируются между контейнерами.
4. Models/configs read-only; results в mounted output directory. No hardcoded workstation paths.
5. Dependency versions, model SHA256 (Secure Hash Algorithm, 256-bit — алгоритм хеширования с результатом длиной 256 бит), image digest и commit находятся в run manifest.
6. Один контейнер вместо двух разрешён с теми же критериями; выбор не даёт дополнительных баллов сам по себе.

Альтернативный симулятор разрешён после общего решения до недели 2: требуется ROS adapter, датчики, contact/GT (Ground Truth — эталонные данные для обучения или оценки) для evaluator, deterministic seeding насколько поддерживается, reset и Docker/headless запуск. Поздняя замена не отменяет interfaces/benchmark.
