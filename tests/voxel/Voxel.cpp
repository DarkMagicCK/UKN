#include "Voxel.h"

namespace ukn {

    namespace voxel {

        BlockStorage::BlockStorage(uint32 size):
        mDimensions(size),
        mModified(false) {

        }

        BlockStorage::~BlockStorage() {

        }

        Block BlockStorage::get(const Position& pos) {
            iterator it = mBlocks.find(pos);
            if(it != mBlocks.end())
                return it->second;
            return Block(0);
        }

        bool BlockStorage::isEmpty() const {
            return mBlocks.empty();
        }

        bool BlockStorage::isModified() const {
            return mModified;
        }

        bool BlockStorage::isFull() const {
            return mBlocks.size() == mDimensions * mDimensions * mDimensions;
        }

        bool BlockStorage::isSolid(const Position& pos) const {
            return mBlocks.find(pos) != mBlocks.end();
        }

        bool BlockStorage::leftVisible(const Position& pos) const {
            return isSolid( Position(pos.x - 1, pos.y, pos.z) );
        }

        bool BlockStorage::rightVisible(const Position& pos) const {
            return isSolid( Position(pos.x + 1, pos.y, pos.z) );
        }

        bool BlockStorage::topVisible(const Position& pos) const {
            return isSolid( Position(pos.x, pos.y + 1, pos.z) );
        }

        bool BlockStorage::rightVisible(const Position& pos) const {
            return isSolid( Position(pos.x, pos.y - 1, pos.z) );
        }

        bool BlockStorage::frontVisible(const Position& pos) const {
            return isSolid( Position(pos.x, pos.y, pos.z + 1) );
        }

        bool BlockStorage::backVisible(const Position& pos) const {
            return isSolid( Position(pos.x, pos.y, pos.z - 1) );
        }

        bool BlockStorage::topBorderFull() const {
            if(isFull())
                return true;
            int y = mDimensions - 1;
            for(int x = 0; x < mDimensions; ++x) {
                for(int z = 0; z < mDimensions; ++z) {
                    if(!isSolid(Position(x, y, z)))
                        return false;
                }
            }
        }

        bool BlockStorage::bottomBorderFull() const {
            if(isFull())
                return true;
            int y = 0;
            for(int x = 0; x < mDimensions; ++x) {
                for(int z = 0; z < mDimensions; ++z) {
                    if(!isSolid(Position(x, y, z)))
                        return false;
                }
            }
        }

        bool BlockStorage::leftBorderFull() const {
            if(isFull())
                return true;
            int x = 0;
            for(int y = 0; y < mDimensions; ++y) {
                for(int z = 0; z < mDimensions; ++z) {
                    if(!isSolid(Position(x, y, z)))
                        return false;
                }
            }
        }

        bool BlockStorage::rightBorderFull() const {
            if(isFull())
                return true;
            int x = mDimensions - 1;
            for(int y = 0; y < mDimensions; ++y) {
                for(int z = 0; z < mDimensions; ++z) {
                    if(!isSolid(Position(x, y, z)))
                        return false;
                }
            }
        }

        bool BlockStorage::frontBorderFull() const {
            if(isFull())
                return true;
            int z = mDimensions - 1;
            for(int y = 0; y < mDimensions; ++y) {
                for(int x = 0; x < mDimensions; ++x) {
                    if(!isSolid(Position(x, y, z)))
                        return false;
                }
            }
        }

        bool BlockStorage::backBorderFull() const {
            if(isFull())
                return true;
            int z = 0;
            for(int y = 0; y < mDimensions; ++y) {
                for(int x = 0; x < mDimensions; ++x) {
                    if(!isSolid(Position(x, y, z)))
                        return false;
                }
            }
        }


        void BlockStorage::ySet(int x, int yStart, int yEnd, int z, const Block& block) {
            if(yStart < yEnd)
                return;
            if(yEnd >= mDimensions)
                return;
            if(yEnd < 0 || yStart < 0)
                return;

            for(int y = yStart; y <= yEnd; ++y) {
                set(Position(x, y, z), block);   
            }
            mModified = true;
        }

        void BlockStorage::empty() {
            mBlocks.clear();
            mModified = false;
        }

        void BlockStorage::set(const Position& pos, const Block& block) {
            if(block.type == 0)
                if(isSolid(pos)) mBlocks.erase(pos);
            else
                mBlocks[pos] = block;
            mModified = true;
        }


        Chunk::Chunk(const Position& pos, uint32 dimension):
        mBlockData(dimension),
        mModified(false),
        mVisible(true),
        mVerticesRendered(0),
        mDimensions(dimension) {
            setPosition(pos);
        }

        Chunk::~Chunk() {

        }

        const UknString& Chunk::getName() const {
            static UknString name(L"Chunk");
            return name;
        }
        
        Box Chunk::getBound() const  {
            return mBound;
        }

        RenderBufferPtr Chunk::getRenderBuffer() const {
            return RenderBufferPtr();
        }
        
        void Chunk::onRenderBegin() {

        }

        void Chunk::onRenderEnd() {

        }
        
        void Chunk::render(const EffectTechniquePtr& technique) {

        }

        void Chunk::setModelMatrix(const Matrix4& mat) {
            mModelMatrix = mat;
        }

        void Chunk::updateBuffers() {

        }

        int Chunk::getVertexCount() {
            return mVertices.left.size() +
                   mVertices.right.size() +
                   mVertices.top.size() +
                   mVertices.bottom.size() +
                   mVertices.front.size() +
                   mVertices.back.size();
        }

        bool Chunk::isObscuredByNeighbors() const {
            if(mNeighbors.left &&
                mNeighbors.right &&
                mNeighbors.top &&
                mNeighbors.bottom &&
                mNeighbors.front &&
                mNeighbors.back) {
                    return mNeighbors.left->rightBorderFull() &&
                        mNeighbors.right->leftBorderFull() &&
                        mNeighbors.top->bottomBorderFull() &&
                        mNeighbors.bottom->topBorderFull() &&
                        mNeighbors.front->backBorderFull() &&
                        mNeighbors.back->frontBorderFull();
            }
        }

        void Chunk::fill() {
            int halfDimension = mDimensions / 2;
            for(int x = 0; x < mDimensions; ++x)
                for(int z = 0; z < mDimensions; ++z)
                    mBlockData.ySet(x, 0, mDimensions, z, Block(1));
        }

        void Chunk::empty() {
            mBlockData.empty();
        }

        void Chunk::setBlock(const Position& pos, const Block& block) {
            mBlockData.set(pos, block);
        }

        Block Chunk::getBlock(const Position& pos) {
            return mBlockData.get(pos);
        }

        void Chunk::setVisible(bool flag) {
            mVisible = flag;
        }

        bool Chunk::isVisible() const {
            return mVisible;
        }

        void Chunk::setPosition(const Position& pos) {
            mPosition = pos;
            
            float halfDimensions = mDimensions / 2.0f;
            mBound =  Box(mist::Vector3(mPosition.x - halfDimensions,
                                     mPosition.y - halfDimensions,
                                     mPosition.z - halfDimensions),
                       mist::Vector3(mPosition.x + halfDimensions,
                                     mPosition.y + halfDimensions,
                                     mPosition.z + halfDimensions));
        }

        Position Chunk::getPosition() const {
            return mPosition;
        }

        uint32 Chunk::getDimensions() const {
            return mDimensions;
        }

        Chunk* Chunk::getLeftChunk() const {
            return mNeighbors.left;
        }

        Chunk* Chunk::getRightChunk() const {
            return mNeighbors.right;
        }

        Chunk* Chunk::getTopChunk() const {
            return mNeighbors.top;
        }

        Chunk* Chunk::getBottomChunk() const {
            return mNeighbors.bottom;
        }

        Chunk* Chunk::getFrontChunk() const {
            return mNeighbors.front;
        }

        Chunk* Chunk::getBackChunk() const {
            return mNeighbors.back;
        }

        void Chunk::setLeftChunk(Chunk* chk) {
            mNeighbors.left = chk;
        }

        void Chunk::setRightChunk(Chunk* chk) {
            mNeighbors.right = chk;
        }

        void Chunk::setTopChunk(Chunk* chk) {
            mNeighbors.top = chk;
        }

        void Chunk::setBottomChunk(Chunk* chk) {
            mNeighbors.bottom = chk;
        }

        void Chunk::setFrontChunk(Chunk* chk) {
            mNeighbors.front = chk;
        }

        void Chunk::setBackChunk(Chunk* chk) {
            mNeighbors.back = chk;
        }

        bool Chunk::topBorderFull() const {
            return mBlockData.topBorderFull();
        }

        bool Chunk::bottomBorderFull() const {
            return mBlockData.bottomBorderFull();
        }

        bool Chunk::leftBorderFull() const {
            return mBlockData.leftBorderFull();
        }

        bool Chunk::rightBorderFull() const {
            return mBlockData.rightBorderFull();
        }

        bool Chunk::frontBorderFull() const {
            return mBlockData.frontBorderFull();
        }

        bool Chunk::backBorderFull() const {
            return mBlockData.backBorderFull();
        }

        void Chunk::checkAndBuildMeshes() {
            if(mModified) {

                updateBuffers();
            }
        }

    }
}