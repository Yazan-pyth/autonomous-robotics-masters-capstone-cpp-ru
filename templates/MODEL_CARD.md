# Model card

Сокращения раскрыты при первом употреблении; [полный словарь терминов](../docs/GLOSSARY.md).

- Название, owner, версия, artifact URL (Uniform Resource Locator — унифицированный указатель ресурса; адрес ресурса) и SHA256 (Secure Hash Algorithm, 256-bit — алгоритм хеширования с результатом длиной 256 бит):
- Назначение, ограничения/OOD (Out of Distribution — данные или условия вне обучающего распределения) и classical fallback:
- Architecture, число параметров, precision, ONNX (Open Neural Network Exchange — открытый формат обмена моделями нейронных сетей) opset (Operator Set — набор и версия операций модели) / LibTorch version:
- Training code commit, dependencies, все training seeds и compute budget:
- Train/validation/test manifests, licenses и подтверждение отсутствия leakage:
- Input names/order/shapes/units, frames, normalization, missing-value policy:
- Output names/shapes/ranges, postprocessing, confidence и NaN (Not a Number — специальное значение «не число») handling:
- Loss/reward, optimizer, stopping criterion, validation-only checkpoint selection:
- Export command, C++ inference command, golden tensor tolerance и test evidence:
- CPU (Central Processing Unit — центральный процессор) latency p50 (50th percentile — 50-й процентиль, медиана)/p95 (95th percentile — 95-й процентиль)/p99 (99th percentile — 99-й процентиль), memory, hardware/thread count; GPU (Graphics Processing Unit — графический процессор) отдельно:
- Per-seed results, failures, fallback fraction и ограничения переносимости:
