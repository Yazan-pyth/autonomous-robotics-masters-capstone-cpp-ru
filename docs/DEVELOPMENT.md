# Разработка и Docker

## Что уже существует

- Два Docker build targets: `simulator` (Gazebo + bridge) и `autonomy` (C++ ROS package).
- Пустой SDF-мир с physics. Он не содержит робота, склада, датчиков или ground truth adapter.
- `capstone_bringup/clock_probe`: C++ подписчик, проверяющий несколько строго возрастающих значений `/clock` и завершающийся с ненулевым кодом при wall-clock timeout.
- Unit test на frozen/regressing/invalid clock и headless Compose smoke в GitHub Actions.

Это инфраструктурная проверка DDS между двумя контейнерами. Студенты реализуют robot/world assets, navigation, datasets/models и final benchmark. CI starter не проверяет качество несуществующих алгоритмов.

## Команды

Требуются Docker Engine + Compose v2 либо Docker Desktop с Linux containers. Первый build скачивает ROS/Gazebo packages; GPU и GUI для smoke не нужны.

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

Локальная ROS 2 среда на Ubuntu:

```bash
source /opt/ros/jazzy/setup.bash
cd ros2_ws
colcon build --cmake-args -DCMAKE_BUILD_TYPE=RelWithDebInfo
colcon test --event-handlers console_direct+
colcon test-result --verbose
```

`clock_probe` сам по себе завершится ошибкой без живого `/clock`; это ожидаемая проверка, не standalone navigation demo.

## Контейнерная архитектура

Compose использует общую bridge network, один `ROS_DOMAIN_ID=42`, middleware CycloneDDS и UDP discovery. `ROS_LOCALHOST_ONLY=0`; host networking, privileged mode и host Docker socket не нужны. Simulator clock bridge внутри simulator container, C++ probe внутри autonomy. Если локальный Docker/network фильтрует multicast, зафиксировать CycloneDDS peer configuration и проверить её на обеих машинах; не считать успешный native запуск доказательством двухконтейнерной связи.

Build base `ros:jazzy-ros-base` обновляется upstream. Starter не обещает bit-for-bit repeatability этого тега. Для итоговой сдачи сохранить image IDs/digests, список пакетов и base digest; использовать один замороженный образ для всей серии. ONNX Runtime/LibTorch и solver пока не установлены — каждая группа добавляет совместимые фиксированные версии и license metadata.

В `docker/start_simulator.sh` shutdown trap останавливает bridge и Gazebo, а ошибка любого дочернего процесса завершает контейнер. В `autonomy` probe имеет 45-секундный steady-clock deadline, чтобы отсутствие `/clock` не приводило к зависанию CI. При создании реального bringup заменить smoke CMD на запуск всех компонентов, сохранив smoke как отдельный тест.

## Требования к финальным контейнерам

1. Clean clone → documented build → один запуск mission без ручной правки файлов контейнера.
2. Headless режим обязателен; GUI/RViz — optional profile. CPU mode обязателен; GPU compose override отдельный.
3. Sensor/command bridges, TF, `/clock`, seed/reset и readiness тестируются между контейнерами.
4. Models/configs read-only; results в mounted output directory. No hardcoded workstation paths.
5. Dependency versions, model SHA256, image digest и commit находятся в run manifest.
6. Один контейнер вместо двух разрешён с теми же критериями; выбор не даёт дополнительных баллов сам по себе.

Альтернативный симулятор разрешён после общего решения до недели 2: требуется ROS adapter, датчики, contact/GT для evaluator, deterministic seeding насколько поддерживается, reset и Docker/headless запуск. Поздняя замена не отменяет interfaces/benchmark.
