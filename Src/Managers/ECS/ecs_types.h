#pragma once
#include <cstdint>
#include <limits>

namespace EDD::ECS {

// 0 — зарезервировано как «невалидный» id.
using entity_id_t = std::uint32_t;
constexpr entity_id_t INVALID_ENTITY_ID = 0;

using generation_t = std::uint16_t;
constexpr generation_t INVALID_GENERATION =
    std::numeric_limits<generation_t>::max();

/* Тип для плотных индексов (например, в SoA-колонках) */
using dense_index_t = std::uint32_t;

/** Максимально допустимое количество различных типов компонентов
 *  (для битовых сигнатур, например). */
constexpr std::size_t MAX_COMPONENT_TYPES = 64;

/** Опционально: размер «страниц» для object-pool’ов компонентов. */
constexpr std::size_t COMPONENT_PAGE_SIZE = 256;

/* ---------------------------------------------------------
 *  Хелперы / Inline-утилиты
 * --------------------------------------------------------- */

/** Простой helper: проверить валидность id. */
inline bool IsValidEntity(entity_id_t id) { return id != INVALID_ENTITY_ID; }

/** Простой helper для generation-ID (если понадобится). */
inline bool IsValidGeneration(generation_t gen) {
  return gen != INVALID_GENERATION;
}
}  // namespace EDD::ECS