#ifndef UKN_TEST_VOXEL_H_
#define UKN_TEST_VOXEL_H_

#include "UKN/UKN.h"

namespace ukn {

    namespace voxel {

        // int position for a block in world
        struct Position {
            int x, y, z;

            Position():
                x(0), y(0), z(0) { }
            Position(int _x, int _y, int _z):
                x(_x), y(_y), z(_z) { }
            Position(const Position& rhs):
                x(rhs.x), y(rhs.y), z(rhs.z) { }
        };

        bool operator == (const Position& p1, const Position& p2) {
            return p1.x == p2.x &&
                    p1.y == p2.y &&
                    p1.z == p2.z;
        }

    }


}

namespace std {

    /* hash function for our block position */
    /* every chunk is 16 * y * 16 in size */
    template<>
    struct hash<ukn::voxel::Position> {
        std::size_t operator()(const ukn::voxel::Position& key) {
            std::size_t hash = key.x * 16 + key.z;
            hash = hash + key.y * 255;
            return hash;
        }
    };

}


namespace ukn {

    namespace voxel {

        enum { EmptyBlock = 0 };

        // a block in chunk
        // currently only holds a 8bit value for block type
        struct Block {
            uint8 type;

            Block():
                type(EmptyBlock) {}
            Block(uint8 _type):
                type(_type) {}
        };

        struct BlockStorage {
        public:
            BlockStorage(uint32 size);
            ~BlockStorage();

            Block get(const Position& pos);

            bool isEmpty() const;
            bool isModified() const;
            bool isFull() const;
            bool isSolid(const Position& pos) const;
            
            bool leftVisible(const Position& pos) const;
            bool rightVisible(const Position& pos) const;
            bool topVisible(const Position& pos) const;
            bool bottomVisible(const Position& pos) const;
            bool frontVisible(const Position& pos) const;
            bool backVisible(const Position& pos) const;

            /* check each border face is full or not */
            bool topBorderFull() const;
            bool bottomBorderFull() const;
            bool leftBorderFull() const;
            bool rightBorderFull() const;
            bool frontBorderFull() const;
            bool backBorderFull() const;

            void ySet(int x, int yStart, int yEnd, int z, const Block& block);
            void empty();
            void set(const Position& pos, const Block& block);

        public:
            typedef std::unordered_map<Position, Block> BlockMap;
            
            typedef BlockMap::iterator iterator;
            typedef BlockMap::const_iterator const_iterator;

            iterator begin() { return mBlocks.begin(); }
            iterator end()  { return mBlocks.end(); }

            const_iterator begin() const { return mBlocks.begin(); }
            const_iterator end() const { return mBlocks.end(); }

            const BlockMap& getBlocks() const { return mBlocks; } 

        private:
            // hash map for fast look up and storage
            BlockMap mBlocks;
            uint32 mDimensions;
            bool mModified;
        };
        
        /* simple template storage class for things that repeats for 
            each face of a box */
        template<typename T>
        struct SixFaceStorage {
            typedef T value_type;

            T front;
            T back;
            T left;
            T right;
            T top;
            T bottom;
        };

        class Chunk;

        template<typename T>
        struct SixFaceStorage<T*> {
            typedef T* value_type;

            T* front;
            T* back;
            T* left;
            T* right;
            T* top;
            T* bottom;

            SixFaceStorage():
                front(ukn::nullptr_t), back(ukn::nullptr_t),
                left(ukn::nullptr_t), right(ukn::nullptr_t),
                top(ukn::nullptr_t), bottom(ukn::nullptr_t) {}
        };


        class Chunk: public ukn::Renderable {
        public:
            Chunk(const Position& pos, uint32 dimension);
            ~Chunk();

        public:
            /* Renderable */
            const UknString& getName() const override;
        
            Box getBound() const override;
            RenderBufferPtr getRenderBuffer() const override;
        
            void onRenderBegin() override;
            void onRenderEnd() override;
        
            void render(const EffectTechniquePtr& technique) override;
            void setModelMatrix(const Matrix4& mat) override;

        public:
            /* Chunk */
            int getVertexCount();

            void fill();
            void empty();

            void setBlock(const Position& pos, const Block& block);
            Block getBlock(const Position& pos);

            void setVisible(bool flag);
            bool isVisible() const;

            void setPosition(const Position& pos);
            Position getPosition() const;

            uint32 getDimensions() const;

            Chunk* getLeftChunk() const;
            Chunk* getRightChunk() const;
            Chunk* getTopChunk() const;
            Chunk* getBottomChunk() const;
            Chunk* getFrontChunk() const;
            Chunk* getBackChunk() const;

            void setLeftChunk(Chunk* chk);
            void setRightChunk(Chunk* chk);
            void setTopChunk(Chunk* chk);
            void setBottomChunk(Chunk* chk);
            void setFrontChunk(Chunk* chk);
            void setBackChunk(Chunk* chk);
            
            bool isObscuredByNeighbors() const;

            bool topBorderFull() const;
            bool bottomBorderFull() const;
            bool leftBorderFull() const;
            bool rightBorderFull() const;
            bool frontBorderFull() const;
            bool backBorderFull() const;
        
        private:
            void checkAndBuildMeshes();
            void updateBuffers();

            BlockStorage mBlockData;

            /* neighboring chunks */
            SixFaceStorage<Chunk*> mNeighbors;
            
            /* vertex datas */
            SixFaceStorage<std::vector<ukn::VertexUVNormal> > mVertices;

            bool mModified;
            bool mVisible;
            uint32 mVerticesRendered;
            uint32 mDimensions;
            Position mPosition;
            Box mBound;
        };

    } 

}

#endif // UKN_TEST_VOXEL_H_