//
//  TMXTiledMap.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/20/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_TMXTiledMap_h
#define Project_Unknown_TMXTiledMap_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/MathUtil.h"
#include "UKN/Serializer.h"
#include "UKN/Renderable.h"
#include "UKN/Util.h"

namespace ukn {
    
    namespace tmx {
        
        /**
         Tiled tmx map reader & renderer
         see www.mapeditor.org for more information about tiled and tmx
         **/

        struct Tile {
            /* local id of the tile, equals to global_id - tileset.first_grid_id */
            uint32 tile_id;
            
            /* texture rectangle of the tile */
            /* 
                x = local_id % (image_width / tile_width) * tile_width
                y = local_id / (image_width / tile_width) * tile_height
             */
            Rectangle tile_texture_rect;
            
            /* bit 32 - 30 in tile data */
            bool flipped_horizontally;
            bool flipped_vertically;
            bool flipped_diagonally;
        };
        
        struct TileSet {
            /* name of the tileset */
            ukn_string name;
            /* first tileset grid id */
            uint32 first_grid;
            
            /* dimensions of the tileset */
            uint32 tile_width;
            uint32 tile_height;
            
            /* though not documented in tmx file format, but shown in examples
                offset between the tile grids */
            uint32 tile_offset_x;
            uint32 tile_offset_y;
            
            /* texture contains the image size */
            TexturePtr image;
            
            /* generated tiles, for faster load */
            /* size = map_width * map_height */
            Tile* tiles;
            
            Tile* tileAt(uint32 local_id) {
                return &tiles[local_id];
            }
        };
        
        struct Layer {
            /* name of the layer */
            ukn_string name;
            
            /* dimensions of the layer, in tile */
            uint32 width;
            uint32 height;
            
            /* id of the tileset to use */
            uint32 tile_set_id;
            
            /* local tiles, maps to tiles in TileSet */
            Tile* tiles;
        };  
        
        enum MapOrientation {
            MO_Isometric,
            MO_orthogonall
        };
        
        class Map: public virtual IRenderable, public virtual IConfigSerializable, public virtual Object {
        public:
            typedef Array<TileSet> TileSetList;
            typedef Array<Layer> LayerList;
            
        public:
            Map();
            Map(const String& map_file);
            virtual ~Map();
            
            const TileSetList&  getTileSets() const;
            const LayerList&    getLayers() const;
            
            SpriteBatchPtr  getMapRenderer() const;
            void            setMapRenderer(SpriteBatchPtr renderer);
            
            // IConfigSerializable
            virtual bool serialize(const ConfigParserPtr& config) override;
            virtual bool deserialize(const ConfigParserPtr& config) override;
            
            // IRenderable
            virtual const String&   getName() override const;
            
            virtual Box             getBound() override const;
            virtual RenderBufferPtr getRenderBuffer() override const;
            
            virtual void onRenderBegin() override;
            virtual void onRenderEnd() override;
            
            virtual void render() override;
            
        private:
            TileSetList mTileSets;
            LayerList mLayers;
            
            SpriteBatchPtr mMapRenderer;
            
            String mName;
            
            uint32 mMapWidth;
            uint32 mMapHeight;
            uint32 mTileWidth;
            uint32 mTileHeight;
            MapOrientation mOrientation;
        };
        
    } // namespace tmx
    
} // namespace ukn

#endif
