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
#include "UKN/Color.h"

namespace ukn {
    
    namespace tmx {
        
        /**
         Tiled tmx map reader & renderer
         see www.mapeditor.org for more information about tiled and tmx
         **/
        
        struct Property {
            ukn_string name;
            ukn_string value;
            
            Property() { }
            Property(const ukn_string& name, const ukn_string& value):
            name(name),
            value(value) { 
            }
        };
        
        struct PropertyContainer {
            typedef std::vector<Property> PropertyList;
            PropertyList properties;
        };
        
        struct TileSet;

        struct Tile {
            /* local id of the tile, equals to global_id - tileset.first_grid_id */
            int32 tile_id;
            
            /* texture rectangle of the tile */
            /* 
                x = local_id % (image_width / tile_width) * tile_width
                y = local_id / (image_width / tile_width) * tile_height
             */
            Rectangle tile_texture_rect;
            Rectangle tile_bounding_rect;
            
            /* bit 32 - 30 in tile data */
            bool flipped_horizontally;
            bool flipped_vertically;
            bool flipped_diagonally;
            
            /* parent tileset id */
            uint32 tileset_id;
            
            /* tile properties */
            PropertyContainer property;
        };
        
        struct TileSet {
            /* name of the tileset */
            ukn_string name;
            /* first tileset grid id */
            int32 first_grid;
            
            /* dimensions of the tileset */
            int32 tile_width;
            int32 tile_height;
            
            /* though not documented in tmx file format, but shown in examples
                offset between the tile grids */
            int32 tile_offset_x;
            int32 tile_offset_y;
            
            /* gird margin and spacing */
            int32 margin;
            int32 spacing;
            
            /* texture contains the image size */
            TexturePtr image;
            
            /* size = map_width * map_height */
            std::vector<Tile> tiles;
            
            /* properties */
            PropertyContainer property;
            
            Tile* tileAt(uint32 local_id) {
                return &tiles[local_id];
            }
        };
        
        class Map;
        
        enum LayerType {
            LT_Layer,
            LT_ObjectGroup,
        };
        
        struct Layer {
            /* type of the layer
                layer or objectgroup
             */
            LayerType type;
            
            /* name of the layer */
            ukn_string name;
            
            /* dimensions of the layer, in tile */
            uint32 width;
            uint32 height;
            
            /* position of the layer */
            int32 x;
            int32 y;
            
            /* opacity of the layer */
            float opacity;
            bool  visible;
            
            const Map* parent;
            
            /* properties */
            PropertyContainer property;
            
            /* local tiles */
            std::vector<Tile> tiles;
            
            Tile& getTileAt(const Vector2& pos);
        };  
        
        struct Object {
            ukn_string name;
            ukn_string type;

            /* coordinate of the object in pixels */
            int32 x;
            int32 y;
            
            /* size of the object in pixels */
            int32 width;
            int32 height;
            
            /* tileid reference of the object, optional */
            int32 gid;
            
            Tile tile;
            TexturePtr image;

            /* properties */
            PropertyContainer property;
        };
        
        /* Obj group is a kind of layer */
        struct ObjectGroup: public Layer {
            /* color of objects */
            Color color;
            
            std::vector<Object> objects;
            
            /* properties */
            PropertyContainer property;
        };
        
        enum MapOrientation {
            MO_Isometric,
            MO_Orthogonal
        };
        
        class Map: public virtual IRenderable, public virtual IConfigSerializable, public virtual Object {
        public:
            typedef Array<TileSet> TileSetList;
            typedef Array<SharedPtr<Layer> > LayerList;
            
        public:
            Map();
            Map(const String& map_file);
            virtual ~Map();
            
            const TileSetList&      getTileSets() const;
            const LayerList&        getLayers() const;
            
            SpriteBatchPtr  getMapRenderer() const;
            void            setMapRenderer(SpriteBatchPtr renderer);
            
            MapOrientation  getOrientation() const;
            
            // in tiles
            uint32          getMapWidth() const;
            uint32          getMapHeight() const;
            
            // ion pixel
            int32           getTileWidth() const;
            int32           getTileHeight() const;
                        
            Tile& getTileAt(uint32 layer_index, const Vector2& pos);

            // in pixel
            void                setMapViewRect(const Rectangle& vr);
            const Rectangle&    getMapViewRect() const;
            
            void                setPosition(const Vector2& pos);
            const Vector2&      getPosition() const;
            
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
            void orthogonalRender();
            void isometricRender();
            
            float2 pixelToTileCoords(float x, float y) const;
            float2 tileToPixelCoords(float x, float y) const;
            
            Tile* getTileWithGid(int32 gid);
            void deserialize_tile_set(TileSet& ts, uint32 ts_id, const ConfigParserPtr& config);
            void parseProperties(PropertyContainer& cont, const ConfigParserPtr& config);
            void parseTileset(const ConfigParserPtr& config);
            void parseLayer(const ConfigParserPtr& config);
            void parseObjectGroup(const ConfigParserPtr& config);
            
            TileSetList mTileSets;
            LayerList mLayers;
            
            SpriteBatchPtr mMapRenderer;
            
            String mName;
            
            uint32  mMapWidth;
            uint32  mMapHeight;
            int32   mTileWidth;
            int32   mTileHeight;
            MapOrientation mOrientation;
            
            Vector2   mPosition;
            Rectangle mViewRect;
        };
        
    } // namespace tmx
    
} // namespace ukn

#endif
