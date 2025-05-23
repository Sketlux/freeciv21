// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: Freeciv21 and Freeciv Contributors

// self
#include "borders.h"

// utility
#include "shared.h"

// common
#include "base.h"
#include "city.h"
#include "extras.h"
#include "fc_types.h"
#include "game.h"
#include "map.h"
#include "tile.h"

/**
   Border radius sq from given border source tile.
 */
int tile_border_source_radius_sq(struct tile *ptile)
{
  struct city *pcity;
  int radius_sq = 0;

  if (BORDERS_DISABLED == game.info.borders) {
    return 0;
  }

  pcity = tile_city(ptile);

  if (pcity) {
    radius_sq = game.info.border_city_radius_sq;
    /* Limit the addition due to the city size. A city size of 60 or more is
     * possible with a city radius of 5 (radius_sq = 26). */
    radius_sq += MIN(city_size_get(pcity), CITY_MAP_MAX_RADIUS_SQ)
                 * game.info.border_size_effect;
  } else {
    extra_type_by_cause_iterate(EC_BASE, pextra)
    {
      struct base_type *pbase = extra_base_get(pextra);

      if (tile_has_extra(ptile, pextra) && territory_claiming_base(pbase)) {
        radius_sq = pbase->border_sq;
        break;
      }
    }
    extra_type_by_cause_iterate_end;
  }

  return radius_sq;
}

/**
   Border source strength
 */
int tile_border_source_strength(struct tile *ptile)
{
  struct city *pcity;
  int strength = 0;

  if (BORDERS_DISABLED == game.info.borders) {
    return 0;
  }

  pcity = tile_city(ptile);

  if (pcity) {
    strength = city_size_get(pcity) + 2;
  } else {
    extra_type_by_cause_iterate(EC_BASE, pextra)
    {
      struct base_type *pbase = extra_base_get(pextra);

      if (tile_has_extra(ptile, pextra) && territory_claiming_base(pbase)) {
        strength = 1;
        break;
      }
    }
    extra_type_by_cause_iterate_end;
  }

  return strength;
}

/**
   Border source strength at tile
 */
int tile_border_strength(struct tile *ptile, struct tile *source)
{
  int full_strength = tile_border_source_strength(source);
  int sq_dist = sq_map_distance(ptile, source);

  if (sq_dist > 0) {
    return full_strength * full_strength / sq_dist;
  } else {
    return FC_INFINITY;
  }
}

/**
   Is given tile source to borders.
 */
bool is_border_source(struct tile *ptile)
{
  if (tile_city(ptile)) {
    return true;
  }

  if (extra_owner(ptile) != nullptr) {
    extra_type_by_cause_iterate(EC_BASE, pextra)
    {
      struct base_type *pbase = extra_base_get(pextra);

      if (tile_has_extra(ptile, pextra) && territory_claiming_base(pbase)) {
        return true;
      }
    }
    extra_type_by_cause_iterate_end;
  }

  return false;
}
