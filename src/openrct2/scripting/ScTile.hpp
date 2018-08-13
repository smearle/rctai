/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../world/Footpath.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "../world/Surface.h"

#include <cstdio>
#include <dukglue/dukglue.h>

namespace OpenRCT2::Scripting
{
    class ScTileElement
    {
    protected:
        rct_tile_element* _element;

    public:
        ScTileElement(rct_tile_element* element)
            : _element(element)
        {
        }

        std::string footpathType_get() const
        {
            switch (_element->GetType())
            {
                case TILE_ELEMENT_TYPE_SURFACE:
                    return "surface";
                case TILE_ELEMENT_TYPE_PATH:
                    return "footpath";
                case TILE_ELEMENT_TYPE_TRACK:
                    return "track";
                case TILE_ELEMENT_TYPE_SMALL_SCENERY:
                    return "small-scenery";
                case TILE_ELEMENT_TYPE_ENTRANCE:
                    return "entrance";
                case TILE_ELEMENT_TYPE_WALL:
                    return "wall";
                case TILE_ELEMENT_TYPE_LARGE_SCENERY:
                    return "large-scenery";
                case TILE_ELEMENT_TYPE_BANNER:
                    return "banner";
                case TILE_ELEMENT_TYPE_CORRUPT:
                    return "openrct2-corrupt";
                default:
                    return "unknown";
            }
        }

        template<typename T> T* getAs()
        {
            return static_cast<T*>(this);
        }

        bool broken_get() const
        {
            return _element->flags & TILE_ELEMENT_FLAG_BROKEN;
        }
        void broken_set(bool value)
        {
            if (value)
            {
                _element->flags |= TILE_ELEMENT_FLAG_BROKEN;
            }
            else
            {
                _element->flags &= ~TILE_ELEMENT_FLAG_BROKEN;
            }
        }

        auto baseHeight_get() const
        {
            return _element->base_height;
        }
        void baseHeight_set(uint8_t newBaseHeight)
        {
            _element->base_height = newBaseHeight;
        }

        auto clearanceHeight_get() const
        {
            return _element->clearance_height;
        }
        void clearanceHeight_set(uint8_t newClearanceHeight)
        {
            _element->clearance_height = newClearanceHeight;
        }

        static void Register(duk_context* ctx);
        template<typename T> static void RegisterSharedProperties(duk_context* ctx);
    };

    class ScSurfaceElement final : public ScTileElement
    {
        uint8_t slope_get()
        {
            return _element->properties.surface.slope & TILE_ELEMENT_SURFACE_SLOPE_MASK;
        }
        void slope_set(uint8_t value)
        {
            // TODO: Give warning when value > TILE_ELEMENT_SURFACE_SLOPE_MASK
            _element->properties.surface.slope &= ~TILE_ELEMENT_SURFACE_SLOPE_MASK;
            _element->properties.surface.slope |= value & TILE_ELEMENT_SURFACE_SLOPE_MASK;
        }

        uint8_t terrain_get()
        {
            return surface_get_terrain(_element);
        }
        void terrain_set(uint8_t value)
        {
            surface_set_terrain(_element, value);
        }

        uint8_t waterHeight_get()
        {
            return surface_get_water_height(_element);
        }
        void waterHeight_set(uint8_t value)
        {
            // TODO: Give warning when value > TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK
            _element->properties.surface.terrain &= ~TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK;
            _element->properties.surface.terrain |= value & TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK;
        }

        uint8_t grassLength_get()
        {
            return _element->properties.surface.grass_length;
        }
        void grassLength_set(uint8_t value)
        {
            // TODO: Give warning when value > GRASS_LENGTH_CLUMPS_2
            _element->properties.surface.grass_length = value;
        }

        uint8_t ownership_get()
        {
            return _element->properties.surface.ownership & 0xF0;
        }
        void ownership_set(uint8_t flags)
        {
            _element->properties.surface.ownership &= ~0xF0;
            _element->properties.surface.ownership |= flags << 4;
        }

        uint8_t parkFences_get()
        {
            return _element->properties.surface.ownership & 0xF0;
        }
        void parkFences_set(uint8_t flags)
        {
            _element->properties.surface.ownership &= ~0x0F;
            _element->properties.surface.ownership |= flags & 0x0F;
        }

    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScSurfaceElement>(ctx);
            dukglue_register_property(ctx, &slope_get, &slope_set, "slope");
            dukglue_register_property(ctx, &terrain_get, &terrain_set, "terrain");
            dukglue_register_property(ctx, &waterHeight_get, &waterHeight_set, "waterHeight");
            dukglue_register_property(ctx, &grassLength_get, &grassLength_set, "grassLength");
            dukglue_register_property(ctx, &ownership_get, &ownership_set, "ownership");
            dukglue_register_property(ctx, &waterHeight_get, &parkFences_set, "parkFences");
        }
    };

    class ScFootpathElement final : public ScTileElement
    {
        class ScFootpathAddition
        {
            class ScFootpathAdditionStatus
            {
            protected:
                rct_tile_element* _element;

            public:
                ScFootpathAdditionStatus(rct_tile_element* element)
                    : _element(element)
                {
                }

                uint8_t north_get()
                {
                    return _element->properties.path.addition_status & 3;
                }
                void north_set(uint8_t value)
                {
                    _element->properties.path.addition_status &= ~3;
                    _element->properties.path.addition_status |= value & 3;
                }

                uint8_t east_get()
                {
                    return _element->properties.path.addition_status & 3;
                }
                void east_set(uint8_t value)
                {
                    _element->properties.path.addition_status &= ~(3 << 2);
                    _element->properties.path.addition_status |= (value & 3) << 2;
                }

                uint8_t south_get()
                {
                    return _element->properties.path.addition_status & 3;
                }
                void south_set(uint8_t value)
                {
                    _element->properties.path.addition_status &= ~(3 << 4);
                    _element->properties.path.addition_status |= (value & 3) << 4;
                }

                uint8_t west_get()
                {
                    return _element->properties.path.addition_status & 3;
                }
                void west_set(uint8_t value)
                {
                    _element->properties.path.addition_status &= ~(3 << 6);
                    _element->properties.path.addition_status |= (value & 3) << 6;
                }

                static void Register(duk_context* ctx)
                {
                    dukglue_register_property(ctx, &north_get, &north_set, "north");
                    dukglue_register_property(ctx, &east_get, &east_set, "east");
                    dukglue_register_property(ctx, &south_get, &south_set, "south");
                    dukglue_register_property(ctx, &west_get, &west_set, "west");
                }
            };

        protected:
            rct_tile_element* _element;
            bool VerifyPathAdditionExists()
            {
                if (!footpath_element_has_path_scenery(_element))
                {
                    // TODO: Show warning in console that the path addition has been removed
                    return false;
                }
                return true;
            }

        public:
            ScFootpathAddition(rct_tile_element* element)
                : _element(element)
            {
            }

            void Remove()
            {
                footpath_element_set_path_scenery(_element, 0);
            }

            uint8_t type_get()
            {
                if (!VerifyPathAdditionExists())
                    return 0;
                return footpath_element_get_path_scenery(_element);
            }
            void type_set(uint8_t value)
            {
                footpath_element_set_path_scenery(_element, value);
            }

            bool isBin_get()
            {
                if (!VerifyPathAdditionExists())
                    return false;

                auto index = footpath_element_get_path_scenery_index(_element);
                auto sceneryEntry = get_footpath_item_entry(index);
                return sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BIN;
            }
            bool isBench_get()
            {
                if (!VerifyPathAdditionExists())
                    return false;

                auto index = footpath_element_get_path_scenery_index(_element);
                auto sceneryEntry = get_footpath_item_entry(index);
                return sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_BENCH;
            }
            bool isLamp_get()
            {
                if (!VerifyPathAdditionExists())
                    return false;

                auto index = footpath_element_get_path_scenery_index(_element);
                auto sceneryEntry = get_footpath_item_entry(index);
                return sceneryEntry->path_bit.flags & PATH_BIT_FLAG_LAMP;
            }
            bool isBreakable_get()
            {
                if (!VerifyPathAdditionExists())
                    return false;

                auto index = footpath_element_get_path_scenery_index(_element);
                auto sceneryEntry = get_footpath_item_entry(index);
                return sceneryEntry->path_bit.flags & PATH_BIT_FLAG_BREAKABLE;
            }
            bool isJumpingFountainWater_get()
            {
                if (!VerifyPathAdditionExists())
                    return false;

                auto index = footpath_element_get_path_scenery_index(_element);
                auto sceneryEntry = get_footpath_item_entry(index);
                return sceneryEntry->path_bit.flags & PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER;
            }
            bool isJumpingFountainSnow_get()
            {
                if (!VerifyPathAdditionExists())
                    return false;

                auto index = footpath_element_get_path_scenery_index(_element);
                auto sceneryEntry = get_footpath_item_entry(index);
                return sceneryEntry->path_bit.flags & PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW;
            }
            bool allowedOnQueue_get()
            {
                if (!VerifyPathAdditionExists())
                    return false;

                auto index = footpath_element_get_path_scenery_index(_element);
                auto sceneryEntry = get_footpath_item_entry(index);
                return sceneryEntry->path_bit.flags & PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE;
            }
            bool allowedOnSlope_get()
            {
                if (!VerifyPathAdditionExists())
                    return false;

                auto index = footpath_element_get_path_scenery_index(_element);
                auto sceneryEntry = get_footpath_item_entry(index);
                return sceneryEntry->path_bit.flags & PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE;
            }
            bool isQueueScreen_get()
            {
                if (!VerifyPathAdditionExists())
                    return false;

                auto index = footpath_element_get_path_scenery_index(_element);
                auto sceneryEntry = get_footpath_item_entry(index);
                return sceneryEntry->path_bit.flags & PATH_BIT_FLAG_IS_QUEUE_SCREEN;
            }

            std::shared_ptr<ScFootpathAdditionStatus> status_get()
            {
                return std::make_shared<ScFootpathAdditionStatus>(_element);
            }

            static void Register(duk_context* ctx)
            {
                // Addition status properties (.status.north, .status.east, etc.)
                ScFootpathAdditionStatus::Register(ctx);

                // Functions
                dukglue_register_method(ctx, &ScFootpathAddition::Remove, "remove");

                // Properties
                dukglue_register_property(ctx, &type_get, &type_set, "type");
                dukglue_register_property(ctx, &isBin_get, nullptr, "isBin");
                dukglue_register_property(ctx, &isBench_get, nullptr, "isBench");
                dukglue_register_property(ctx, &isLamp_get, nullptr, "isLamp");
                dukglue_register_property(ctx, &isBreakable_get, nullptr, "isBreakable");
                dukglue_register_property(ctx, &isJumpingFountainWater_get, nullptr, "isJumpingFountainWater");
                dukglue_register_property(ctx, &isJumpingFountainSnow_get, nullptr, "isJumpingFountainSnow");
                dukglue_register_property(ctx, &allowedOnQueue_get, nullptr, "allowedOnQueue");
                dukglue_register_property(ctx, &allowedOnSlope_get, nullptr, "allowedOnSlope");
                dukglue_register_property(ctx, &isQueueScreen_get, nullptr, "isQueueScreen");
                dukglue_register_property(ctx, &status_get, nullptr, "status");
            }
        };

        uint8_t footpathType_get()
        {
            return footpath_element_get_type(_element);
        }
        void type_set(uint8_t index)
        {
            auto entry = get_footpath_entry(index);
            if (entry == nullptr)
            {
                // TODO: Give warning (most likely only happens when index > MAX_PATH_OBJECTS)
                return;
            }
            footpath_element_set_type(_element, index);
        }

        bool isSloped_get()
        {
            return footpath_element_is_sloped(_element);
        }
        void isSlope_set(bool value)
        {
            footpath_element_set_sloped(_element, value);
        }

        bool isQueue_get()
        {
            return footpath_element_is_queue(_element);
        }

        std::shared_ptr<ScFootpathAddition> addition_get()
        {
            if (!footpath_element_has_path_scenery(_element))
                return nullptr;

            return std::make_shared<ScFootpathAddition>(_element);
        }

        uint8_t edges_get()
        {
            return footpath_element_get_edges(_element);
        }
        void edges_set(uint8_t value)
        {
            footpath_element_set_edges(_element, value);
        }

        uint8_t corners_get()
        {
            return footpath_element_get_corners(_element);
        }
        void corners_set(uint8_t value)
        {
            footpath_element_set_corners(_element, value);
        }

        uint8_t rideIndex_get()
        {
            if (!footpath_element_is_queue(_element))
            {
                // TODO: Show warning that "path is not a queue"
            }
            return _element->properties.path.ride_index;
        }

    public:
        static void Register(duk_context* ctx)
        {
            // Register shared properties (baseheight, clearance height, etc.)
            RegisterSharedProperties<ScFootpathElement>(ctx);

            // Register path addition properties (.addition.isBench, .addition.isLamp, etc.)
            ScFootpathAddition::Register(ctx);

            // Specific properties
            dukglue_register_property(ctx, &footpathType_get, &type_set, "footpathType");
            dukglue_register_property(ctx, &isSloped_get, &isSlope_set, "isSloped");
            dukglue_register_property(ctx, &isQueue_get, nullptr, "isQueue");
            dukglue_register_property(ctx, &addition_get, nullptr, "addition");
            dukglue_register_property(ctx, &edges_get, &edges_set, "edges");
            dukglue_register_property(ctx, &corners_get, &corners_set, "corners");
            dukglue_register_property(ctx, &rideIndex_get, nullptr, "rideIndex");
        }
    };

    class ScTrackElement final : public ScTileElement
    {
        auto track_ride_index_get() const
        {
            return _element->properties.track.ride_index;
        }

    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScTrackElement>(ctx);
            dukglue_register_property(ctx, &ScTrackElement::track_ride_index_get, nullptr, "rideIndex");
        }
    };

    class ScSmallSceneryElement final : public ScTileElement
    {
    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScSmallSceneryElement>(ctx);
        }
    };

    class ScEntranceElement final : public ScTileElement
    {
    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScEntranceElement>(ctx);
        }
    };

    class ScWallElement final : public ScTileElement
    {
    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScWallElement>(ctx);
        }
    };

    class ScLargeSceneryElement final : public ScTileElement
    {
    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScLargeSceneryElement>(ctx);
        }
    };

    class ScBannerElement final : public ScTileElement
    {
    public:
        static void Register(duk_context* ctx)
        {
            RegisterSharedProperties<ScBannerElement>(ctx);
        }
    };

    void ScTileElement::Register(duk_context* ctx)
    {
        dukglue_register_method(ctx, &ScTileElement::getAs<ScSurfaceElement>, "asSurface");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScFootpathElement>, "asFootpath");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScTrackElement>, "asTrack");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScSmallSceneryElement>, "asSmallScenery");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScEntranceElement>, "asEntrance");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScWallElement>, "asWall");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScLargeSceneryElement>, "asLargeScenery");
        dukglue_register_method(ctx, &ScTileElement::getAs<ScBannerElement>, "asBanner");

        // Shared properties
        RegisterSharedProperties<ScTileElement>(ctx);

        // Register type specific properties
        ScSurfaceElement ::Register(ctx);
        ScFootpathElement::Register(ctx);
        ScTrackElement ::Register(ctx);
        ScSmallSceneryElement ::Register(ctx);
        ScEntranceElement ::Register(ctx);
        ScWallElement ::Register(ctx);
        ScLargeSceneryElement ::Register(ctx);
        ScBannerElement ::Register(ctx);
    }

    template<typename T> void ScTileElement::RegisterSharedProperties(duk_context* ctx)
    {
        // When you register a member function of a parent class, it belongs to the parent and cannot be accessed by its
        // children. To get around this issue, we need to force the function pointer types to be of the class we want them to be
        // available for. This needs to be done for every child.
        dukglue_register_property(
            ctx, static_cast<std::string (T::*)() const>(&ScTileElement::footpathType_get), nullptr, "type");
        dukglue_register_property(
            ctx, static_cast<bool (T::*)() const>(&ScTileElement::broken_get),
            static_cast<void (T::*)(bool)>(&ScTileElement::broken_set), "broken");
        dukglue_register_property(
            ctx, static_cast<uint8_t (T::*)() const>(&ScTileElement::baseHeight_get),
            static_cast<void (T::*)(uint8_t)>(&ScTileElement::baseHeight_set), "baseHeight");
        dukglue_register_property(
            ctx, static_cast<uint8_t (T::*)() const>(&ScTileElement::clearanceHeight_get),
            static_cast<void (T::*)(uint8_t)>(&ScTileElement::clearanceHeight_set), "clearanceHeight");
    }

    class ScTile
    {
    private:
        rct_tile_element* _first;
        size_t _count = 0;

    public:
        ScTile(rct_tile_element* first)
            : _first(first)
        {
            _count = 0;
            if (first != nullptr)
            {
                auto element = first;
                do
                {
                    _count++;
                } while (!(element++)->IsLastForTile());
            }
        }

        size_t numElements_get()
        {
            return _count;
        }

        std::shared_ptr<ScTileElement> getElement(size_t index)
        {
            if (index >= _count)
                return nullptr;
            return std::make_shared<ScTileElement>(&_first[index]);
        }

        // This is not a good idea as the array is generated each time elements is accessed and
        // chances are scripts will do tile.elements[0], tile.elements[1]...
        // std::vector<std::shared_ptr<ScTileElement>> elements_get()
        // {
        //     std::vector<std::shared_ptr<ScTileElement>> elements;
        //     for (size_t i = 0; i < _count; i++)
        //     {
        //         elements.push_back(std::make_shared<ScTileElement>(&_first[i]));
        //     }
        //     return elements;
        // }

        static void Register(duk_context* ctx)
        {
            // dukglue_register_property(ctx, &ScTile::elements_get, nullptr, "elements");
            dukglue_register_property(ctx, &ScTile::numElements_get, nullptr, "numElements");
            dukglue_register_method(ctx, &ScTile::getElement, "getElement");
        }
    };
} // namespace OpenRCT2::Scripting