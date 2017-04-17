#ifndef SIMPLEFONT_H
#define SIMPLEFONT_H

#include "../functions/defs.h"
#include "Bits.h"

namespace grynca {

    // TODO: predelat Line na Text a SDLText
    //       ktery si bude uvnitr resit radky sam

    class F8x8 {
    public:
        // 8x8 fixed size font
        static u8 getCharRow(u32 char_id, u32 row_id);

        class Text {
        public:
            Text(const std::string& text);

            // size in pixels
            u32 getHeight()const;
            u32 getWidth()const;

            // will be called as f(u32 x, u32 y)
            template <typename LoopFunc>
            void loopTextPixels(const LoopFunc& f);

            const std::string& getText()const;
            void setText(const std::string& text);
        protected:
            void setTextBitmap_();

            u32 lines_cnt_;
            u32 line_size_;
            std::string text_;
            Bits text_bitmap_;  //1d bitmap for line (row after row)
        };

#if USE_SDL2 == 1
        // not meant to be used as virtual Line
        class SDL2Text : public Text {
        public:
            SDL2Text(SDL_Renderer* renderer, const std::string& text = "");
            ~SDL2Text();

            void setText(const std::string& text);

            void draw(u8 r, u8 g, u8 b, u8 a, i32 x, i32 y);
            void draw(u8 r, u8 g, u8 b, u8 a, i32 x, i32 y, u32 w, u32 h);      // can specify size of destination rect
        private:
            void setTexture_();

            SDL_Renderer* renderer_;
            SDL_Texture* texture_;
        };
#endif

    };
}

#include "SimpleFont.inl"
#endif //SIMPLEFONT_H
