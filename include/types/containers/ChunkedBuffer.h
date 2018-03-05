#ifndef CHUNKEDBUFFER_H
#define CHUNKEDBUFFER_H

#include "fast_vector.h"
#include "../Type.h"

namespace grynca {

    class ChunkedBuffer {
        static constexpr u32 target_chunk_bytes_ = 65536;
    public:
        explicit ChunkedBuffer();
        explicit ChunkedBuffer(u32 item_size);

        ~ChunkedBuffer();

        void init(u32 item_size);

        u32 getItemSize()const;
        u32 getSize()const;
        u32 getCapacity()const;
        u32 getChunksCount()const;
        u32 getOccupiedChunksCount()const;
        u32 getChunkSize()const;
        const u8* getChunkData(u32 chunk_id)const;

        void resize(u32 n);
        void reserve(u32 n);

        u8* accItem(u32 item_id);
        const u8* getItem(u32 item_id) const;

        // LoopFunc(u8* item)
        template <typename LoopFunc>
        void forEach(const LoopFunc& lf)const;
        // LoopFunc(u8* item, u32 item_id)
        template <typename LoopFunc>
        void forEachWithId(const LoopFunc& lf)const;

        void clear();
        bool isEmpty()const;
    private:
        static u32 calcChunkSize_(u32 item_size);

        fast_vector<u8*> blocks_;
        u32 item_size_;
        u32 chunk_size_;
        u32 size_;
        u32 capacity_;
    };

}

#include "ChunkedBuffer.inl"
#endif //CHUNKEDBUFFER_H
