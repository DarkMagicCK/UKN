#include "UKN/Vertex.h"
#include "mist/Logger.h"

#include "UKN/GraphicBuffer.h"

namespace ukn {

#define ADVANCE_VOID_PTR(ptr, size) (void*)(((char*)ptr) + size)
    
    /* a helper class that maps a vertex buffer in constructor
        and unmap it in destrcutor
        
     */
    struct VertexMapper {
        VertexMapper(const GraphicBufferPtr& _buffer, const vertex_elements_type& _elements);
        ~VertexMapper();

        friend struct VertexIterator;
        struct VertexIterator {
            VertexIterator();
            VertexIterator(const VertexIterator& rhs);
            VertexIterator(void* ptr, VertexMapper* parent);

            VertexIterator& operator++();
            VertexIterator operator=(const VertexIterator& rhs);

            bool operator!=(const VertexIterator& rhs) const;
            bool operator==(const VertexIterator& rhs) const;

            void* operator*() const;
            void* curr() const;

            VertexUsage currUsage() const;

            // advance to next element in the vertex
            void nextElement();

            void* getElement(VertexUsage usage, uint32 index = 0);
           
            void* base_ptr;
            void* curr_ptr;
            uint32 index;
            VertexMapper* parent;
        };
        
        typedef VertexIterator iterator;

        iterator begin();
        iterator end();

        // total size of vertex elements
        // = vertex_elements_type.size()
        uint32 element_count();

        void* data;
        GraphicBufferPtr buffer;
        const vertex_elements_type& elements;
        uint32 element_size;
    };
  

    VertexMapper::VertexMapper(const GraphicBufferPtr& _buffer, const vertex_elements_type& _elements):
    buffer(_buffer),
    elements(_elements) {
            if(buffer)
                data = buffer->map();
            else
                log_error(L"VertexMapper: invalid graphic buffer ptr");

            if(!data)
                log_error(L"VertexMapper:: failed to map vertex data");

            element_size = GetVertexElementsTotalSize(elements);
    }

    VertexMapper::~VertexMapper() {
        if(buffer) {
            buffer->unmap();
        }
    }

    VertexMapper::VertexIterator::VertexIterator():
        base_ptr(0),
        curr_ptr(0),
        parent(0) {

    }

    VertexMapper::VertexIterator::VertexIterator(const VertexIterator& rhs):
        base_ptr(rhs.base_ptr),
        curr_ptr(rhs.curr_ptr),
        parent(rhs.parent) {

    }

    VertexMapper::VertexIterator::VertexIterator(void* _ptr, VertexMapper* _parent):
        base_ptr(_ptr),
        curr_ptr(_ptr),
        parent(_parent) {

    }

    VertexMapper::VertexIterator& VertexMapper::VertexIterator::operator++() {
        if(base_ptr && parent) {
            base_ptr = ADVANCE_VOID_PTR(base_ptr, parent->element_size);
            curr_ptr = base_ptr;
        }
        return *this;
    }

    void VertexMapper::VertexIterator::nextElement() {
        if(base_ptr && parent &&
            index < parent->element_count()) {
            curr_ptr = ADVANCE_VOID_PTR(curr_ptr, parent->elements[index].size());
            index += 1;
        }
    }

    void* VertexMapper::VertexIterator::operator*() const {
        return this->base_ptr;
    }

    void* VertexMapper::VertexIterator::curr() const {
        return this->curr_ptr;
    }

    VertexUsage VertexMapper::VertexIterator::currUsage() const {
        if(index < parent->elements.size())
            return parent->elements[index].usage;
        return VU_Unknown;
    }

    void* VertexMapper::VertexIterator::getElement(VertexUsage usage, uint32 uindex) {
        uint32 index = 0;
        uint32 offset = 0;
        while(index < parent->elements.size()) {
            const VertexElement& element = parent->elements[index];
            if(element.usage == usage &&
                element.usage_index == uindex) {
                return ADVANCE_VOID_PTR(base_ptr, offset);
            }
            offset += element.size();
            ++index;
        }
        return 0;
    }

    VertexMapper::VertexIterator VertexMapper::VertexIterator::operator=(const VertexIterator& rhs) {
        if(*this != rhs) {
            this->base_ptr = rhs.base_ptr;
            this->curr_ptr = rhs.curr_ptr;
            this->parent = rhs.parent;
        }
        return *this;
    }

    bool VertexMapper::VertexIterator::operator!=(const VertexIterator& rhs) const {
        return !(*this == rhs);
    }

    bool VertexMapper::VertexIterator::operator==(const VertexIterator& rhs) const {
        return this->base_ptr == rhs.base_ptr && this->parent == rhs.parent;
    }

    VertexMapper::iterator VertexMapper::begin() {
        return VertexIterator(data, this);
    }

    VertexMapper::iterator VertexMapper::end() {
        if(buffer) {
            void* endptr = ADVANCE_VOID_PTR(data, element_size * buffer->count());
            return VertexIterator(endptr, this);
        } 
        return begin();
    }

    uint32 VertexMapper::element_count() {
        return (uint32)elements.size();
    }


}