#ifndef USTRING_H
#define USTRING_H

// Copyright (c) 2017 Jakob Riedle (DuffsDevice)
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE AUTHOR 'AS IS' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include <memory> // for std::unique_ptr
#include <cstring> // for std::memcpy, std::strlen
#include <string> // for std::string
#include <cstddef> // for char32_t, ptrdiff_t
#include <iostream> // for ostream/istream overloadings

class ustring
{
public:

    typedef size_t					size_type;
    typedef ptrdiff_t				difference_type;
    typedef char32_t				value_type;
    typedef value_type				const_reference;
    constexpr static size_type		npos = -1;

    friend std::ostream& operator<<( std::ostream& , const ustring& );
		friend std::istream& operator>>( std::istream& , ustring& );

private:

    class utf8_codepoint_reference
    {
    private:

        size_type		index;
        ustring*	instance;

    public:

        //! Ctor
        utf8_codepoint_reference( size_type index , ustring* instance ) :
                index( index )
                , instance( instance )
        {}

        //! Cast to wide char
        operator value_type() const { return static_cast<const ustring*>(this->instance)->at( index ); }

        //! Assignment operator
        utf8_codepoint_reference& operator=( value_type ch ){
            instance->replace( index , ch );
            return *this;
        }
    };

    class utf8_codepoint_raw_reference
    {
    private:

        size_type		raw_index;
        ustring*	instance;

    public:

        //! Ctor
        utf8_codepoint_raw_reference( size_type raw_index , ustring* instance ) :
                raw_index( raw_index )
                , instance( instance )
        {}

        //! Cast to wide char
        operator value_type() const { return static_cast<const ustring*>(this->instance)->raw_at( this->raw_index ); }

        //! Assignment operator
        utf8_codepoint_raw_reference& operator=( value_type ch ){
            this->instance->raw_replace( this->raw_index , this->instance->get_index_bytes( this->raw_index ) , ustring( 1 , ch ) );
            return *this;
        }
    };

    class iterator_base
    {
        friend class ustring;

    public:

        typedef ustring::value_type			value_type;
        typedef ustring::difference_type	difference_type;
        typedef utf8_codepoint_raw_reference	reference;
        typedef void*							pointer;
        typedef std::forward_iterator_tag		iterator_category;

        bool operator==( const iterator_base& it ) const { return this->raw_index == it.raw_index; }
        bool operator!=( const iterator_base& it ) const { return this->raw_index != it.raw_index; }

        //! Ctor
        iterator_base( difference_type raw_index , ustring* instance ) :
                raw_index( raw_index )
                , instance( instance )
        {}

        //! Default function
        iterator_base() = default;
        iterator_base( const iterator_base& ) = default;
        iterator_base& operator=( const iterator_base& ) = default;

        // Getter for the iterator index
        difference_type get_index() const { return raw_index; }
        ustring* get_instance() const { return instance; }

    protected:

        difference_type	raw_index;
        ustring*	instance;

        void advance( difference_type n ){
            if( n > 0 )
                do
                    increment();
                while( --n > 0 );
            else
                while( n++ < 0 )
                    decrement();
        }

        void increment(){
            this->raw_index += this->instance->get_index_bytes( this->raw_index );
        }

        void decrement(){
            this->raw_index -= this->instance->get_index_pre_bytes( this->raw_index );
        }

        utf8_codepoint_raw_reference get_reference() const {
            return this->instance->raw_at( this->raw_index );
        }
        value_type get_value() const {
            return static_cast<const ustring*>(this->instance)->raw_at( this->raw_index );
        }
    };

public:

    typedef utf8_codepoint_reference	reference;

    class iterator : public iterator_base
    {
    public:

        //! Ctor
        iterator( difference_type raw_index , ustring* instance ) :
                iterator_base( raw_index , instance )
        {}

        //! Default Functions
        iterator() = default;
        iterator( const iterator& ) = default;
        iterator& operator=( const iterator& ) = default;

        //! Delete ctor from const iterator types
        iterator( const class const_iterator& ) = delete;
        iterator( const class const_reverse_iterator& ) = delete;

        //! Increase the Iterator by one
        iterator&  operator++(){ // prefix ++iter
            increment();
            return *this;
        }
        iterator& operator++( int ){ // postfix iter++
            iterator tmp{ raw_index , instance };
            increment();
            return *this;
        }

        //! Decrease the iterator by one
        iterator&  operator--(){ // prefix --iter
            decrement();
            return *this;
        }
        iterator& operator--( int ){ // postfix iter--
            iterator tmp{ raw_index , instance };
            decrement();
            return *this;
        }

        //! Increase the Iterator n times
        iterator operator+( difference_type n ) const {
            iterator it{*this};
            it.advance( n );
            return it;
        }
        iterator& operator+=( difference_type n ){
            this->advance( n );
            return *this;
        }

        //! Decrease the Iterator n times
        iterator operator-( difference_type n ) const {
            iterator it{*this};
            it.advance( -n );
            return it;
        }
        iterator& operator-=( difference_type n ){
            this->advance( -n );
            return *this;
        }

        //! Returns the value of the codepoint behind the iterator
        utf8_codepoint_raw_reference operator*() const { return get_reference(); }
    };

    class const_iterator : public iterator
    {
    public:

        //! Ctor
        const_iterator( difference_type raw_index , const ustring* instance ) :
                iterator( raw_index , const_cast<ustring*>(instance) )
        {}

        //! Ctor from non const
        const_iterator( const iterator& it ) :
                iterator( it.get_index() , it.get_instance() )
        {}

        //! Default Functions
        const_iterator() = default;
        const_iterator( const const_iterator& ) = default;
        const_iterator& operator=( const const_iterator& ) = default;

        //! Returns the (raw) value behind the iterator
        value_type operator*() const { return get_value(); }
    };

    class reverse_iterator : public iterator_base
    {
    public:

        //! Ctor
        reverse_iterator( difference_type raw_index , ustring* instance ) :
                iterator_base( raw_index , instance )
        {}

        //! Ctor from normal iterator
        reverse_iterator( const iterator& it ) :
                iterator_base( it.get_index() , it.get_instance() )
        {}

        //! Default Functions
        reverse_iterator() = default;
        reverse_iterator( const reverse_iterator& ) = default;
        reverse_iterator& operator=( const reverse_iterator& ) = default;

        //! Delete ctor from const iterator types
        reverse_iterator( const const_iterator& ) = delete;
        reverse_iterator( const class const_reverse_iterator& ) = delete;

        //! Increase the iterator by one
        reverse_iterator&  operator++(){ // prefix ++iter
            decrement();
            return *this;
        }
        reverse_iterator& operator++( int ){ // postfix iter++
            reverse_iterator tmp{ raw_index , instance };
            decrement();
            return *this;
        }

        //! Decrease the Iterator by one
        reverse_iterator&  operator--(){ // prefix --iter
            increment();
            return *this;
        }
        reverse_iterator& operator--( int ){ // postfix iter--
            reverse_iterator tmp{ raw_index , instance };
            increment();
            return *this;
        }

        //! Increase the Iterator n times
        reverse_iterator operator+( difference_type n ) const {
            reverse_iterator it{*this};
            it.advance( -n );
            return it;
        }
        reverse_iterator& operator+=( difference_type n ){
            this->advance( -n );
            return *this;
        }

        //! Decrease the Iterator n times
        reverse_iterator operator-( difference_type n ) const {
            reverse_iterator it{*this};
            it.advance( n );
            return it;
        }
        reverse_iterator& operator-=( difference_type n ){
            this->advance( n );
            return *this;
        }

        //! Returns the value of the codepoint behind the iterator
        utf8_codepoint_raw_reference operator*() const { return get_reference(); }

        //! Get the underlying iterator instance
        iterator base() const {
            return iterator( raw_index , instance );
        }
    };

    class const_reverse_iterator : public reverse_iterator
    {
    public:

        //! Ctor
        const_reverse_iterator( difference_type raw_index , const ustring* instance ) :
                reverse_iterator( raw_index , const_cast<ustring*>(instance) )
        {}

        //! Ctor from non const
        const_reverse_iterator( const reverse_iterator& it ) :
                reverse_iterator( it.get_index() , it.get_instance() )
        {}

        //! Ctor from normal iterator
        const_reverse_iterator( const const_iterator& it ) :
                reverse_iterator( it.get_index() , it.get_instance() )
        {}

        //! Default Functions
        const_reverse_iterator() = default;
        const_reverse_iterator( const const_reverse_iterator& ) = default;
        const_reverse_iterator& operator=( const const_reverse_iterator& ) = default;

        //! Returns the (raw) value behind the iterator
        value_type operator*() const { return get_value(); }

        //! Get the underlying iterator instance
        const_iterator base() const {
            return const_iterator( raw_index , instance );
        }
    };

private:

    //! Attributes
    char*			buffer;
    size_type		buffer_len;
    size_type		string_len;
    size_type*		indices_of_multibyte;
    size_type		indices_len;
    mutable bool	misformatted;
    bool			static_buffer;

    //! Frees the internal buffer of indices and sets it to the supplied value
    void reset_indices( size_type* indices_of_multibyte = nullptr , size_type indices_len = 0 ){
        if( this->indices_of_multibyte )
            delete[] this->indices_of_multibyte;
        this->indices_len = indices_len;
        this->indices_of_multibyte = indices_of_multibyte;
    }

    //! Frees the internal buffer of indices
    void reset_buffer( char* buffer = nullptr , size_type buffer_len = 0 ){
        if( this->buffer && !this->static_buffer )
            delete[] this->buffer;
        this->static_buffer = false;
        this->buffer_len = buffer_len;
        this->buffer = buffer;
    }

    /**
     * Returns an std::string with the UTF-8 BOM prepended
     */
    std::string cpp_str_bom() const ;

    /**
     * Returns the number of bytes to expect behind this one (including this one) that belong to this utf8 char
     */
    unsigned char get_num_bytes_of_utf8_char( const char* data , size_type first_byte_index ) const ;

    /**
     * Returns the number of bytes to expect before this one (including this one) that belong to this utf8 char
     */
    unsigned char get_num_bytes_of_utf8_char_before( const char* data , size_type current_byte_index ) const ;

    /**
     * Returns the number of bytes to expect behind this one that belong to this utf8 char
     */
    static unsigned char get_num_bytes_of_utf8_codepoint( value_type codepoint )
    {
        if( codepoint <= 0x7F )
            return 1;
        else if( codepoint <= 0x7FF )
            return 2;
        else if( codepoint <= 0xFFFF )
            return 3;
        else if( codepoint <= 0x1FFFFF )
            return 4;
        else if( codepoint <= 0x3FFFFFF )
            return 5;
        else if( codepoint <= 0x7FFFFFFF )
            return 6;
        return 0;
    }

    //! Returns the actual byte index of the supplied codepoint index
    size_type get_actual_index( size_type codepoint_index ) const ;

    /**
     * Get the byte index of the last codepoint
     */
    size_type back_index() const { return size() - get_index_pre_bytes( size() ); }
    /**
     * Get the byte index of the index behind the last codepoint
     */
    size_type end_index() const { return size(); }

    /**
     * Decodes a given input of rle utf8 data to a
     * unicode codepoint and returns the number of bytes it used
     */
    unsigned char decode_utf8( const char* data , value_type& codepoint ) const ;

    /**
     * Encodes a given codepoint to a character buffer of at least 7 bytes
     * and returns the number of bytes it used
     */
    static unsigned char encode_utf8( value_type codepoint , char* dest );

    /**
     * Counts the number of bytes required
     * to hold the given wide character string.
     * numBytes is set to the number of multibyte
     * codepoints (ones that consume mor than 1 byte).
     */
    static size_type get_num_required_bytes( const value_type* lit , size_type& numMultibytes );

    /**
     * Counts the number of codepoints
     * that are built up of the supplied amout of bytes
     */
    size_type get_num_resulting_codepoints( size_type byte_start , size_type byte_count ) const ;

    /**
     * Counts the number of bytes
     * required to hold the supplied amount of codepoints starting at the supplied byte index
     */
    size_type get_num_resulting_bytes( size_type byte_start , size_type codepoint_count ) const ;

    //! Ctor from buffer and indices
    ustring( char* buffer , size_type buffer_len , size_type string_len , size_type* indices_of_multibyte , size_type indices_len ) :
            buffer( buffer )
            , buffer_len( buffer_len )
            , string_len( string_len )
            , indices_of_multibyte( indices_of_multibyte )
            , indices_len( indices_len )
            , misformatted( false )
            , static_buffer( false )
    {}

public:

    /**
     * Default Ctor
     *
     * @note Creates an Instance of type ustring that is empty
     */
    ustring() :
            buffer( nullptr )
            , buffer_len( 0 )
            , string_len( 0 )
            , indices_of_multibyte( nullptr )
            , indices_len( 0 )
            , misformatted( false )
            , static_buffer( false )
    {}

    /**
     * Contructor taking an utf8 sequence and the maximum length to read from it (in number of codepoints)
     *
     * @note	Creates an Instance of type ustring that holds the given utf8 sequence
     * @param	str		The UTF-8 sequence to fill the ustring with
     * @param	len		(Optional) The maximum number of codepoints to read from the sequence
     */
    ustring( const char* str , size_type len = ustring::npos );


    /**
     * Contructor taking an std::string
     *
     * @note	Creates an Instance of type ustring that holds the given data sequence
     * @param	str		The byte data, that will be interpreted as UTF-8
     */
    ustring( std::string str ) :
            ustring( str.c_str() )
    {}

    /**
     * Named constructor that constructs a ustring from a string literal without
     * parsing the input string for UTF-8 sequences
     *
     * @param	str		The ascii sequence that will be imported
     */
    static const ustring ascii_constant( const char* str ){
        size_type	len = std::strlen( str ) + 1;
        ustring wstr = ustring( const_cast<char*>( str ) , len , len - 1 , nullptr , 0 );
        wstr.static_buffer = true;
        return (ustring&&)wstr;
    }
    /**
     * Named constructor that constructs a ustring from an std::string without
     * parsing the input string for UTF-8 sequences
     *
     * @param	str		The ascii sequence that will be imported
     */
    static ustring ascii_constant( const std::string& str ){
        size_type	len = str.length() + 1;
        char*	buffer = new char[len];
        std::memcpy( buffer , str.data() , len );
        return ustring( buffer , len , len - 1 , nullptr , 0 );
    }


    /**
     * Contructor that fills itself with a certain amount of codepoints
     *
     * @note	Creates an Instance of type ustring that gets filled with 'n' codepoints
     * @param	n		The number of codepoints generated
     * @param	ch		The codepoint that the whole buffer will be set to
     */
    ustring( size_type n , value_type ch );
    /**
     * Contructor that fills itself with a certain amount of characters
     *
     * @note	Creates an Instance of type ustring that gets filled with 'n' characters
     * @param	n		The number of characters generated
     * @param	ch		The characters that the whole buffer will be set to
     */
    ustring( size_type n , char ch ) :
            ustring( n , (value_type)ch )
    {}


    /**
     * Copy Constructor that copies the supplied ustring to construct itself
     *
     * @note	Creates an Instance of type ustring that has the exact same data as 'str'
     * @param	str		The ustring to copy from
     */
    ustring( const ustring& str );
    /**
     * Move Constructor that moves the supplied ustring content into the new ustring
     *
     * @note	Creates an Instance of type ustring that takes all data from 'str'
     * 			The supplied ustring is invalid afterwards and may not be used anymore
     * @param	str		The ustring to move from
     */
    ustring( ustring&& str );


    /**
     * Contructor taking a wide codepoint literal that will be copied to construct this ustring
     *
     * @note	Creates an Instance of type ustring that holds the given codepoints
     *			The data itself will be first converted to UTF-8
     * @param	str		The codepoint sequence to fill the ustring with
     */
    ustring( const value_type* str );


    /**
     * Destructor
     *
     * @note	Destructs a ustring at the end of its lifetime releasing all held memory
     */
    ~ustring(){
        this->reset_indices();
        this->reset_buffer();
    }


    /**
     * Copy Assignment operator that sets the ustring to a copy of the supplied one
     *
     * @note	Assigns a copy of 'str' to this ustring deleting all data that previously was in there
     * @param	str		The ustring to copy from
     * @return	A reference to the string now holding the data (*this)
     */
    ustring& operator=( const ustring& str );
    /**
     * Move Assignment operator that moves all data out of the supplied and into this ustring
     *
     * @note	Moves all data from 'str' into this ustring deleting all data that previously was in there
     *			The supplied ustring is invalid afterwards and may not be used anymore
     * @param	str		The ustring to move from
     * @return	A reference to the string now holding the data (*this)
     */
    ustring& operator=( ustring&& str );


    /**
     * Swaps the contents of this ustring with the supplied one
     *
     * @note	Swaps all data with the supplied ustring
     * @param	str		The ustring to swap contents with
     */
    void swap( ustring& str ){
        std::swap( *this , str );
    }


    /**
     * Clears the content of this ustring
     *
     * @note	Resets the data to an empty string ("")
     */
    void clear(){
        this->string_len = 0;
        this->misformatted = false;
        reset_buffer();
        reset_indices();
    }


    /**
     * Returns whether there is an UTF-8 error in the string
     *
     * @return	True, if there is an encoding error, false, if the contained data is properly encoded
     */
    bool is_misformatted() const { return this->misformatted; }


    /**
     * Returns the codepoint at the supplied index
     *
     * @param	n	The codepoint index of the codepoint to receive
     * @return	The Codepoint at position 'n'
     */
    value_type at( size_type n ) const ;
    value_type at( size_type n ){
        return utf8_codepoint_reference( n , this );
    }
    /**
     * Returns the codepoint at the supplied byte position
     *
     * @note	As this access is raw, that is not looking up for the actual byte position,
     *			it is very fast
     * @param	byte_index	The byte position of the codepoint to receive
     * @return	The codepoint at the supplied position
     */
    utf8_codepoint_raw_reference raw_at( size_type byte_index ){
        return utf8_codepoint_raw_reference( byte_index , this );
    }
    value_type raw_at( size_type byte_index ) const {
        if( !this->requires_unicode() )
            return this->buffer[byte_index];
        value_type dest;
        decode_utf8( this->buffer + byte_index , dest );
        return dest;
    }


    /**
     * Returns an iterator pointing to the supplied codepoint index
     *
     * @param	n	The index of the codepoint to get the iterator to
     * @return	An iterator pointing to the specified codepoint index
     */
    iterator get( size_type n ){ return iterator( get_actual_index(n) , this ); }
    const_iterator get( size_type n ) const { return const_iterator( get_actual_index(n) , this ); }
    /**
     * Returns an iterator pointing to the codepoint at the supplied byte position
     *
     * @note	As this access is raw, that is not looking up for the actual byte position,
     *			it is very fast
     * @param	n	The byte position of the codepoint to get the iterator to
     * @return	An iterator pointing to the specified byte position
     */
    iterator raw_get( size_type n ){ return iterator( n , this ); }
    const_iterator raw_get( size_type n ) const { return const_iterator( n , this ); }


    /**
     * Returns a reverse iterator pointing to the supplied codepoint index
     *
     * @param	n	The index of the codepoint to get the reverse iterator to
     * @return	A reverse iterator pointing to the specified codepoint index
     */
    reverse_iterator rget( size_type n ){ return reverse_iterator( get_actual_index(n) , this ); }
    const_reverse_iterator rget( size_type n ) const { return const_reverse_iterator( get_actual_index(n) , this ); }
    /**
     * Returns a reverse iterator pointing to the codepoint at the supplied byte position
     *
     * @note	As this access is raw, that is not looking up for the actual byte position,
     *			it is very fast
     * @param	n	The byte position of the codepoint to get the reverse iterator to
     * @return	A reverse iterator pointing to the specified byte position
     */
    reverse_iterator raw_rget( size_type n ){ return reverse_iterator( n , this ); }
    const_reverse_iterator raw_rget( size_type n ) const { return const_reverse_iterator( n , this ); }


    /**
     * Returns a reference to the codepoint at the supplied index
     *
     * @param	n	The codepoint index of the codepoint to receive
     * @return	A reference wrapper to the codepoint at position 'n'
     */
    utf8_codepoint_reference operator[]( size_type n ){ return utf8_codepoint_reference( n , this ); }
    value_type operator[]( size_type n ) const { return at( n ); }
    /**
     * Returns a reference to the codepoint at the supplied byte position
     *
     * @note	As this access is raw, that is not looking up for the actual byte position,
     *			it is very fast
     * @param	n	The byte position of the codepoint to receive
     * @return	A reference wrapper to the codepoint at byte position 'n'
     */
    utf8_codepoint_raw_reference operator()( size_type n ){ return utf8_codepoint_raw_reference( n , this ); }
    value_type operator()( size_type n ) const { return raw_at( n ); }


    /**
     * Get the raw data contained in this ustring
     *
     * @note	Returns the UTF-8 formatted content of this ustring
     * @return	UTF-8 formatted data, trailled by a '\0'
     */
    const char* c_str() const { return this->buffer ? this->buffer : ""; }
    const char* data() const { return this->buffer; }


    /**
     * Get the raw data contained in this ustring wrapped by an std::string
     *
     * @note	Returns the UTF-8 formatted content of this ustring
     * @return	UTF-8 formatted data, wrapped inside an std::string
     */
    std::string cpp_str( bool prepend_bom = false ) const { return prepend_bom ? cpp_str_bom() : std::string( this->c_str() ); }


    /**
     * Get the number of codepoints in this ustring
     *
     * @note	Returns the number of codepoints that are taken care of
     *			For the number of bytes, @see size()
     * @return	Number of codepoints (not bytes!)
     */
    size_type length() const { return this->string_len; }


    /**
     * Get the number of bytes that are used by this ustring
     *
     * @note	Returns the number of bytes required to hold the contained wide string,
     *			That is, without counting the trailling '\0'
     * @return	Number of bytes (not codepoints!)
     */
    size_type size() const { return std::max<size_type>( 1 , this->buffer_len ) - 1; }


    /**
     * Check, whether this ustring is empty
     *
     * @note	Returns True, if this ustring is empty, that also is comparing true with ""
     * @return	True, if this ustring is empty, false if its length is >0
     */
    bool empty() const { return !this->string_len; }


    /**
     * Check whether the data inside this ustring cannot be iterated by an std::string
     *
     * @note	Returns true, if the ustring has codepoints that exceed 7 bits to be stored
     * @return	True, if there are UTF-8 formatted byte sequences,
     *			false, if there are only ascii characters (<128) inside
     */
    bool requires_unicode() const { return this->indices_len > 0; }


    /**
     * Returns the number of multibytes within this ustring
     *
     * @return	The number of codepoints that take more than one byte
     */
    size_type get_num_multibytes() const { return this->indices_len; }


    /**
     * Get a wide string literal representing this UTF-8 string
     *
     * @note	As the underlying UTF-8 data has to be converted to a wide character array and
     *			as this array has to be allocated, this method is quite slow, that is O(n)
     * @return	A wrapper class holding the wide character array
     */
    std::unique_ptr<value_type[]> wide_literal() const ;


    /**
     * Get an iterator to the beginning of the ustring
     *
     * @return	An iterator class pointing to the beginning of this ustring
     */
    iterator begin(){ return iterator( 0 , this ); }
    const_iterator begin() const { return const_iterator( 0 , this ); }
    /**
     * Get an iterator to the end of the ustring
     *
     * @return	An iterator class pointing to the end of this ustring, that is pointing behind the last codepoint
     */
    iterator end(){ return iterator( end_index() , this ); }
    const_iterator end() const { return const_iterator( end_index() , this ); }


    /**
     * Get a reverse iterator to the end of this ustring
     *
     * @return	A reverse iterator class pointing to the end of this ustring,
     *			that is exactly to the last codepoint
     */
    reverse_iterator rbegin(){ return reverse_iterator( back_index() , this ); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator( back_index() , this ); }
    /**
     * Get a reverse iterator to the beginning of this ustring
     *
     * @return	A reverse iterator class pointing to the end of this ustring,
     *			that is pointing before the first codepoint
     */
    reverse_iterator rend(){ return reverse_iterator( -1 , this ); }
    const_reverse_iterator rend() const { return const_reverse_iterator( -1 , this ); }


    /**
     * Get a const iterator to the beginning of the ustring
     *
     * @return	A const iterator class pointing to the beginning of this ustring,
     * 			which cannot alter things inside this ustring
     */
    const_iterator cbegin() const { return const_iterator( 0 , this ); }
    /**
     * Get an iterator to the end of the ustring
     *
     * @return	A const iterator class, which cannot alter this ustring, pointing to
     *			the end of this ustring, that is pointing behind the last codepoint
     */
    const_iterator cend() const { return const_iterator( end_index() , this ); }


    /**
     * Get a const reverse iterator to the end of this ustring
     *
     * @return	A const reverse iterator class, which cannot alter this ustring, pointing to
     *			the end of this ustring, that is exactly to the last codepoint
     */
    const_reverse_iterator crbegin() const { return const_reverse_iterator( back_index() , this ); }
    /**
     * Get a const reverse iterator to the beginning of this ustring
     *
     * @return	A const reverse iterator class, which cannot alter this ustring, pointing to
     *			the end of this ustring, that is pointing before the first codepoint
     */
    const_reverse_iterator crend() const { return const_reverse_iterator( -1 , this ); }


    /**
     * Returns a reference to the first codepoint in the ustring
     *
     * @return	A reference wrapper to the first codepoint in the ustring
     */
    utf8_codepoint_raw_reference front(){ return utf8_codepoint_raw_reference( 0 , this ); }
    value_type front() const { return raw_at( 0 ); }
    /**
     * Returns a reference to the last codepoint in the ustring
     *
     * @return	A reference wrapper to the last codepoint in the ustring
     */
    utf8_codepoint_raw_reference back(){ return utf8_codepoint_raw_reference( empty() ? 0 : back_index() , this ); }
    value_type back() const { return raw_at( empty() ? 0 : back_index() ); }


    /**
     * Replace a codepoint of this ustring by a number of codepoints
     *
     * @param	index	The codpoint index to be replaced
     * @param	repl	The wide character that will be used to replace the codepoint
     * @param	n		The number of codepoint that will be inserted
     *					instead of the one residing at position 'index'
     * @return	A reference to this ustring, which now has the replaced part in it
     */
    ustring& replace( size_type index , value_type repl , size_type n = 1 ){
        replace( index , 1 , ustring( n , repl ) );
        return *this;
    }
    /**
     * Replace a number of codepoints of this ustring by a number of other codepoints
     *
     * @param	index	The codpoint index at which the replacement is being started
     * @param	len		The number of codepoints that are being replaced
     * @param	repl	The wide character that will be used to replace the codepoints
     * @param	n		The number of codepoint that will replace the old ones
     * @return	A reference to this ustring, which now has the replaced part in it
     */
    ustring& replace( size_type index , size_type len , value_type repl , size_type n = 1 ){
        replace( index , len , ustring( n , repl ) );
        return *this;
    }
    /**
     * Replace a range of codepoints by a number of codepoints
     *
     * @param	first	An iterator pointing to the first codepoint to be replaced
     * @param	last	An iterator pointing to the codepoint behind the last codepoint to be replaced
     * @param	repl	The wide character that will be used to replace the codepoints in the range
     * @param	n		The number of codepoint that will replace the old ones
     * @return	A reference to this ustring, which now has the replaced part in it
     */
    ustring& replace( iterator first , iterator last , value_type repl , size_type n = 1 ){
        raw_replace( first.raw_index , last.raw_index - first.raw_index , ustring( n , repl ) );
        return *this;
    }
    /**
     * Replace a range of codepoints with the contents of the supplied ustring
     *
     * @param	first	An iterator pointing to the first codepoint to be replaced
     * @param	last	An iterator pointing to the codepoint behind the last codepoint to be replaced
     * @param	repl	The ustring to replace all codepoints in the range
     * @return	A reference to this ustring, which now has the replaced part in it
     */
    ustring& replace( iterator first , iterator last , const ustring& repl ){
        raw_replace( first.raw_index , last.raw_index - first.raw_index , repl );
        return *this;
    }
    /**
     * Replace a number of codepoints of this ustring with the contents of the supplied ustring
     *
     * @param	index	The codpoint index at which the replacement is being started
     * @param	len		The number of codepoints that are being replaced
     * @param	repl	The ustring to replace all codepoints in the range
     * @return	A reference to this ustring, which now has the replaced part in it
     */
    ustring& replace( size_type index , size_type count , const ustring& repl ){
        size_type actualStartIndex = get_actual_index( index );
        raw_replace(
                actualStartIndex
                , count == ustring::npos ? ustring::npos : get_num_resulting_bytes( actualStartIndex , count )
                , repl
        );
        return *this;
    }
    /**
     * Replace a number of bytes of this ustring with the contents of the supplied ustring
     *
     * @note	As this function is raw, that is not having to compute byte indices,
     *			it is much faster than the codepoint-base replace function
     * @param	byte_index	The byte position at which the replacement is being started
     * @param	byte_count	The number of bytes that are being replaced
     * @param	repl		The ustring to replace all bytes inside the range
     * @return	A reference to this ustring, which now has the replaced part in it
     */
    void raw_replace( size_type byte_index , size_type byte_count , const ustring& repl );


    /**
     * Prepend the supplied ustring to this ustring
     *
     * @param	appendix	The ustring to be prepended
     * @return	A reference to this ustring, which now has the supplied string prepended
     */
    ustring& prepend( const ustring& prependix ){
        raw_replace( 0 , 0 , prependix );
        return *this;
    }

    /**
     * Appends the supplied ustring to the end of this ustring
     *
     * @param	appendix	The ustring to be appended
     * @return	A reference to this ustring, which now has the supplied string appended
     */
    ustring& append( const ustring& appendix ){
        raw_replace( size() , 0 , appendix );
        return *this;
    }
    ustring& operator+=( const ustring& summand ){
        raw_replace( size() , 0 , summand );
        return *this;
    }


    /**
     * Appends the supplied codepoint to the end of this ustring
     *
     * @param	ch	The codepoint to be appended
     * @return	A reference to this ustring, which now has the supplied codepoint appended
     */
    ustring& push_back( value_type ch ){
        raw_replace( size() , 0 , ustring( 1 , ch ) );
        return *this;
    }

    /**
     * Prepends the supplied codepoint to this ustring
     *
     * @param	ch	The codepoint to be prepended
     * @return	A reference to this ustring, which now has the supplied codepoint prepended
     */
    ustring& push_front( value_type ch ){
        raw_replace( 0 , 0 , ustring( 1 , ch ) );
        return *this;
    }


    /**
     * Adds the supplied ustring to a copy of this ustring
     *
     * @param	summand		The ustring to be added
     * @return	A reference to the newly created ustring, which now has the supplied string appended
     */
    ustring operator+( const ustring& summand ) const {
        ustring str = *this;
        str.append( summand );
        return std::move( str );
    }


    /**
     * Inserts a given codepoint into this ustring at the supplied codepoint index
     *
     * @param	pos		The codepoint index to insert at
     * @param	ch		The codepoint to be inserted
     * @return	A reference to this ustring, with the supplied codepoint inserted
     */
    ustring& insert( size_type pos , value_type ch ){
        replace( pos , 0 , ustring( 1 , ch ) );
        return *this;
    }
    /**
     * Inserts a given ustring into this ustring at the supplied codepoint index
     *
     * @param	pos		The codepoint index to insert at
     * @param	str		The ustring to be inserted
     * @return	A reference to this ustring, with the supplied ustring inserted
     */
    ustring& insert( size_type pos , const ustring& str ){
        replace( pos , 0 , str );
        return *this;
    }
    /**
     * Inserts a given codepoint into this ustring at the supplied iterator position
     *
     * @param	it	The iterator position to insert at
     * @param	ch	The codepoint to be inserted
     * @return	A reference to this ustring, with the supplied codepoint inserted
     */
    ustring& insert( iterator it , value_type ch ){
        raw_replace( it.raw_index , 0 , ustring( 1 , ch ) );
        return *this;
    }
    /**
     * Inserts a given ustring into this ustring at the supplied iterator position
     *
     * @param	it	The iterator position to insert at
     * @param	ch	The ustring to be inserted
     * @return	A reference to this ustring, with the supplied codepoint inserted
     */
    ustring& insert( iterator it , const ustring& str ){
        raw_replace( it.raw_index , 0 , str );
        return *this;
    }
    /**
     * Inserts a given ustring into this ustring at the supplied byte position
     *
     * @note	As this function is raw, that is without having to compute
     *			actual byte indices, it is much faster that insert()
     * @param	pos		The byte position index to insert at
     * @param	str		The ustring to be inserted
     * @return	A reference to this ustring, with the supplied ustring inserted
     */
    ustring& raw_insert( size_type pos , const ustring& str ){
        raw_replace( pos , 0 , str );
        return *this;
    }


    /**
     * Erases the codepoint at the supplied iterator position
     *
     * @param	pos		The iterator pointing to the position being erased
     * @return	A reference to this ustring, which now has the codepoint erased
     */
    ustring& erase( iterator pos ){
        raw_replace( pos.raw_index , get_index_bytes( pos.raw_index )  , ustring() );
        return *this;
    }
    /**
     * Erases the codepoints inside the supplied range
     *
     * @param	first	An iterator pointing to the first codepoint to be erased
     * @param	last	An iterator pointing to the codepoint behind the last codepoint to be erased
     * @return	A reference to this ustring, which now has the codepoints erased
     */
    ustring& erase( iterator first , iterator last ){
        raw_replace( first.raw_index , last.raw_index - first.raw_index + get_index_bytes( last.raw_index ), ustring() );
        return *this;
    }
    /**
     * Erases a portion of this string
     *
     * @param	pos		The codepoint index to start eraseing from
     * @param	len		The number of codepoints to be erased from this ustring
     * @return	A reference to this ustring, with the supplied portion erased
     */
    ustring& erase( size_type pos , size_type len = 1 ){
        replace( pos , len , ustring() );
        return *this;
    }
    /**
     * Erases a byte range of this string
     *
     * @note	As this function is raw, that is without having to compute
     *			actual byte indices, it is much faster that erase()
     * @param	pos		The byte position index to start erasing from
     * @param	len		The number of bytes to be erased from the ustring
     * @return	A reference to this ustring, with the supplied bytes erased
     */
    ustring& raw_erase( size_type pos , size_type len ){
        raw_replace( pos , len , ustring() );
        return *this;
    }


    /**
     * Returns a portion of the ustring
     *
     * @param	first	An iterator pointing to the first codepoint to be included in the substring
     * @param	last	An iterator pointing to the codepoint behind the last codepoint in the substring
     * @return	The ustring holding the specified range
     */
    ustring substr( iterator first , iterator last ) const {
        size_type byte_count = last.raw_index - first.raw_index;
        return raw_substr( first.raw_index , byte_count , get_num_resulting_codepoints( first.raw_index , byte_count ) );
    }
    /**
     * Returns a portion of the ustring
     *
     * @param	pos		The codepoint index that should mark the start of the substring
     * @param	len		The number codepoints to be included within the substring
     * @return	The ustring holding the specified codepoints
     */
    ustring substr( size_type pos , size_type len = ustring::npos ) const
    {
        difference_type	actualStartIndex = get_actual_index( pos );

        if( len == ustring::npos )
            return raw_substr( actualStartIndex , ustring::npos );

        difference_type	actualEndIndex = len ? get_actual_index( pos + len ) : actualStartIndex;

        return raw_substr( actualStartIndex , actualEndIndex - actualStartIndex , len );
    }
    /**
     * Returns a portion of the ustring (indexed on byte-base)
     *
     * @note	As this function is raw, that is without having to compute
     *			actual byte indices, it is much faster that substr()
     * @param	byte_index		The byte position where the substring shall start
     * @param	byte_count		The number of bytes that the substring shall have
     * @param	numCodepoints	(Optional) The number of codepoints
     *							the substring will have, in case this is already known
     * @return	The ustring holding the specified bytes
     */
    ustring raw_substr( size_type byte_index , size_type byte_count , size_type numCodepoints ) const ;
    ustring raw_substr( size_type byte_index , size_type byte_count = ustring::npos ) const {
        if( byte_count == ustring::npos )
            byte_count = size() - byte_index;
        return raw_substr( byte_index , byte_count , get_num_resulting_codepoints( byte_index , byte_count ) );
    }


    /**
     * Finds a specific codepoint inside the ustring starting at the supplied codepoint index
     *
     * @param	ch				The codepoint to look for
     * @param	start_codepoint	The index of the first codepoint to start looking from
     * @return	The codepoint index where and if the codepoint was found or ustring::npos
     */
    size_type find( value_type ch , size_type start_codepoint = 0 ) const ;
    /**
     * Finds a specific pattern within the ustring starting at the supplied codepoint index
     *
     * @param	ch				The codepoint to look for
     * @param	start_codepoint	The index of the first codepoint to start looking from
     * @return	The codepoint index where and if the pattern was found or ustring::npos
     */
    size_type find( const ustring& pattern , size_type start_codepoint = 0 ) const {
        if( start_codepoint >= length() )
            return ustring::npos;
        size_type actual_start = get_actual_index( start_codepoint );
        const char* result = std::strstr( buffer + actual_start , pattern.c_str() );
        if( !result )
            return ustring::npos;
        return start_codepoint + get_num_resulting_codepoints( actual_start , result - (buffer + actual_start) );
    }
    /**
     * Finds a specific pattern within the ustring starting at the supplied codepoint index
     *
     * @param	ch				The codepoint to look for
     * @param	start_codepoint	The index of the first codepoint to start looking from
     * @return	The codepoint index where and if the pattern was found or ustring::npos
     */
    size_type find( const char* pattern , size_type start_codepoint = 0 ) const {
        if( start_codepoint >= length() )
            return ustring::npos;
        size_type actual_start = get_actual_index( start_codepoint );
        const char* result = std::strstr( buffer + actual_start , pattern );
        if( !result )
            return ustring::npos;
        return start_codepoint + get_num_resulting_codepoints( actual_start , result - (buffer + actual_start) );
    }
    /**
     * Finds a specific codepoint inside the ustring starting at the supplied byte position
     *
     * @param	ch			The codepoint to look for
     * @param	start_byte	The byte position of the first codepoint to start looking from
     * @return	The byte position where and if the codepoint was found or ustring::npos
     */
    size_type raw_find( value_type ch , size_type start_byte = 0 ) const ;
    /**
     * Finds a specific pattern within the ustring starting at the supplied byte position
     *
     * @param	needle		The pattern to look for
     * @param	start_byte	The byte position of the first codepoint to start looking from
     * @return	The byte position where and if the pattern was found or ustring::npos
     */
    size_type raw_find( const ustring& pattern , size_type start_byte = 0 ) const {
        if( start_byte >= size() )
            return ustring::npos;
        const char* result = std::strstr( buffer + start_byte , pattern.c_str() );
        if( !result )
            return ustring::npos;
        return result - buffer;
    }
    /**
     * Finds a specific pattern within the ustring starting at the supplied byte position
     *
     * @param	needle		The pattern to look for
     * @param	start_byte	The byte position of the first codepoint to start looking from
     * @return	The byte position where and if the pattern was found or ustring::npos
     */
    size_type raw_find( const char* pattern , size_type start_byte = 0 ) const {
        if( start_byte >= size() )
            return ustring::npos;
        const char* result = std::strstr( buffer + start_byte , pattern );
        if( !result )
            return ustring::npos;
        return result - buffer;
    }

    /**
     * Finds the last occourence of a specific codepoint inside the
     * ustring starting backwards at the supplied codepoint index
     *
     * @param	ch				The codepoint to look for
     * @param	start_codepoint	The index of the first codepoint to start looking from (backwards)
     * @return	The codepoint index where and if the codepoint was found or ustring::npos
     */
    size_type rfind( value_type ch , size_type start_codepoint = ustring::npos ) const ;
    /**
     * Finds the last occourence of a specific codepoint inside the
     * ustring starting backwards at the supplied byte index
     *
     * @param	ch				The codepoint to look for
     * @param	start_codepoint	The byte index of the first codepoint to start looking from (backwards)
     * @return	The codepoint index where and if the codepoint was found or ustring::npos
     */
    size_type raw_rfind( value_type ch , size_type start_byte = ustring::npos ) const ;

    //! Find characters in string
    size_type find_first_of( const value_type* str , size_type start_codepoint = 0 ) const ;
    size_type raw_find_first_of( const value_type* str , size_type start_byte = 0 ) const ;
    size_type find_last_of( const value_type* str , size_type start_codepoint = ustring::npos ) const ;
    size_type raw_find_last_of( const value_type* str , size_type start_byte = ustring::npos ) const ;

    //! Find absence of characters in string
    size_type find_first_not_of( const value_type* str , size_type start_codepoint = 0 ) const ;
    size_type raw_find_first_not_of( const value_type* str , size_type start_byte = 0 ) const ;
    size_type find_last_not_of( const value_type* str , size_type start_codepoint = ustring::npos ) const ;
    size_type raw_find_last_not_of( const value_type* str , size_type start_byte = ustring::npos ) const ;


    //! Removes the last codepoint in the ustring
    ustring& pop_back(){
        size_type pos = back_index();
        raw_replace( pos , get_index_bytes( pos ) , ustring() );
        return *this;
    }

    /**
     * Compares this ustring to the supplied one
     *
     * @return	0	They compare equal
     *			<0	Either the value of the first character that does not match is lower in
     *			the compared string, or all compared characters match but the compared string is shorter.
     *			>0	Either the value of the first character that does not match is greater in
     *			the compared string, or all compared characters match but the compared string is longer.
     */
    difference_type compare( const ustring& str ) const ;

    //! Returns true, if the supplied string compares equal to this one
    bool equals( const ustring& str ) const { return equals( str.c_str() ); }
    bool equals( const std::string& str ) const { return equals( str.c_str() ); }
    bool equals( const char* str ) const ;
    bool equals( const value_type* str ) const ;

    //! Compare this ustring to another string
    bool operator==( const ustring& str ) const { return equals( str ); }
    bool operator!=( const ustring& str ) const { return !equals( str ); }
    bool operator==( const char* str ) const { return equals( str ); }
    bool operator!=( const char* str ) const { return !equals( str ); }
    bool operator==( const value_type* str ) const { return equals( str ); }
    bool operator!=( const value_type* str ) const { return !equals( str ); }
    bool operator==( const std::string& str ) const { return equals( str ); }
    bool operator!=( const std::string& str ) const { return !equals( str ); }

    bool operator>( const ustring& str ) const { return str.compare( *this ) > 0; }
    bool operator>=( const ustring& str ) const { return str.compare( *this ) >= 0; }
    bool operator<( const ustring& str ) const { return str.compare( *this ) < 0; }
    bool operator<=( const ustring& str ) const { return str.compare( *this ) <= 0; }


    //! Get the number of bytes of codepoint in ustring
    unsigned char get_codepoint_bytes( size_type codepoint_index ) const {
        return get_num_bytes_of_utf8_char( this->buffer , get_actual_index(codepoint_index) );
    }
    unsigned char get_index_bytes( size_type byte_index ) const {
        return get_num_bytes_of_utf8_char( this->buffer , byte_index );
    }


    //! Get the number of bytes before a codepoint, that build up a new codepoint
    unsigned char get_codepoint_pre_bytes( size_type codepoint_index ) const {
        return this->string_len > codepoint_index ? get_num_bytes_of_utf8_char_before( this->buffer , get_actual_index(codepoint_index) ) : 1;
    }
    unsigned char get_index_pre_bytes( size_type byte_index ) const {
        return this->buffer_len > byte_index ? get_num_bytes_of_utf8_char_before( this->buffer , byte_index ) : 1;
    }



    //! Friend iterator difference computation functions
    friend int				operator-( const const_iterator& lhs , const const_iterator& rhs );
    friend int				operator-( const const_reverse_iterator& lhs , const const_reverse_iterator& rhs );
};


//! Compare two iterators
static inline bool operator>( const ustring::const_iterator& lhs , const ustring::const_iterator& rhs ){ return lhs.get_index() > rhs.get_index(); }
static inline bool operator>( const ustring::const_reverse_iterator& lhs , const ustring::const_reverse_iterator& rhs ){ return lhs.get_index() < rhs.get_index(); }
static inline bool operator>=( const ustring::const_iterator& lhs , const ustring::const_iterator& rhs ){ return lhs.get_index() >= rhs.get_index(); }
static inline bool operator>=( const ustring::const_reverse_iterator& lhs , const ustring::const_reverse_iterator& rhs ){ return lhs.get_index() <= rhs.get_index(); }
static inline bool operator<( const ustring::const_iterator& lhs , const ustring::const_iterator& rhs ){ return lhs.get_index() < rhs.get_index(); }
static inline bool operator<( const ustring::const_reverse_iterator& lhs , const ustring::const_reverse_iterator& rhs ){ return lhs.get_index() > rhs.get_index(); }
static inline bool operator<=( const ustring::const_iterator& lhs , const ustring::const_iterator& rhs ){ return lhs.get_index() <= rhs.get_index(); }
static inline bool operator<=( const ustring::const_reverse_iterator& lhs , const ustring::const_reverse_iterator& rhs ){ return lhs.get_index() >= rhs.get_index(); }

ustring operator+(const ustring::value_type* lhs, ustring&& rhs) { return std::move(rhs.insert(0, lhs)); }
ustring operator+(const ustring::value_type* lhs, const ustring& rhs) { return ustring(lhs)+rhs; }

////! Compute distance between iterators
//extern int	operator-( const ustring::const_iterator& lhs , const ustring::const_iterator& rhs );
//extern int	operator-( const ustring::const_reverse_iterator& lhs , const ustring::const_reverse_iterator& rhs );

std::ostream& operator<<( std::ostream& stream , const ustring& str ){
	return stream << str.c_str();
}

std::istream& operator>>( std::istream& stream , ustring& str ){
	std::string tmp;
	stream >> tmp;
	str = move(tmp);
	return stream;
}

inline ustring::ustring( ustring::size_type number , ustring::value_type ch ) :
        string_len( number )
        , indices_of_multibyte( ch <= 0x7F && number ? nullptr : new ustring::size_type[number] )
        , indices_len( ch <= 0x7F ? 0 : number )
        , misformatted( false )
        , static_buffer( false )
{
    if( !number ){
        this->buffer = nullptr;
        this->buffer_len = 0;
        return;
    }

    unsigned char num_bytes = get_num_bytes_of_utf8_codepoint( ch );
    this->buffer_len		= ( ch ? number * num_bytes : 0 ) + 1;
    this->buffer			= new char[this->buffer_len];

    if( num_bytes == 1 )
        while( number-- > 0 )
            this->buffer[number] = ch;
    else
    {
        char representation[6];
        encode_utf8( ch , representation );

        while( number-- > 0 )
        {
            unsigned char byte = num_bytes;
            while( byte-- > 0 )
                this->buffer[ number * num_bytes + byte ] = representation[byte];
            this->indices_of_multibyte[number] = number * num_bytes;
        }
    }

    // Set trailling zero
    this->buffer[ this->buffer_len - 1 ] = 0;
}

inline ustring::ustring( const char* str , size_type len ) :
        ustring()
{
    // Reset some attributes, or else 'get_num_bytes_of_utf8_char' will not work, since it thinks the string is empty
    this->buffer_len = -1;
    this->indices_len = -1;

    if(str)
    {
        if (!str[0]) {
            clear();
        }
        else {
            size_type		num_multibytes = 0;
            size_type		num_bytes = 0;
            unsigned char	num_bytes_to_skip = 0;

            // Count Multibytes
            for( ; str[num_bytes] && this->string_len < len ; num_bytes++ )
            {
                this->string_len++; // Count number of codepoints

                if( static_cast<unsigned char>(str[num_bytes]) <= 0x7F ) // lead bit is zero, must be a single ascii
                    continue;

                // Compute the number of bytes to skip
                num_bytes_to_skip = get_num_bytes_of_utf8_char( str , num_bytes );

                num_multibytes++; // Increase number of occoured multibytes

                // Check if the string doesn't end just right inside the multibyte part!
                while( --num_bytes_to_skip > 0 )
                {
                    if( !str[num_bytes+1] || ( static_cast<unsigned char>(str[num_bytes+1]) & 0xC0 ) != 0x80 ){
                        this->misformatted = true;
                        num_multibytes--; // Decrease counter, since that sequence is apparently no ++valid++ utf8
                        break;
                    }
                    num_bytes++;
                }
            }

            // Initialize buffer
            this->buffer_len = num_bytes + 1;
            this->buffer = new char[this->buffer_len];
            std::memcpy( this->buffer , str , this->buffer_len );

            // initialize indices table
            this->indices_len = num_multibytes;
            this->indices_of_multibyte = this->indices_len ? new size_type[this->indices_len] : nullptr;

            size_type multibyteIndex = 0;

            // Fill Multibyte Table
            for( size_type index = 0 ; index < num_bytes ; index++ )
            {
                if( static_cast<unsigned char>(buffer[index]) <= 0x7F ) // lead bit is zero, must be a single ascii
                    continue;

                // Compute number of bytes to skip
                num_bytes_to_skip = get_num_bytes_of_utf8_char( buffer , index ) - 1;

                this->indices_of_multibyte[multibyteIndex++] = index;

                if( this->misformatted )
                    while( num_bytes_to_skip-- > 0 )
                    {
                        if( index + 1 == num_bytes || ( static_cast<unsigned char>(buffer[index+1]) & 0xC0 ) != 0x80 ){
                            multibyteIndex--; // Remove the byte position from the list, since its no ++valid++ multibyte sequence
                            break;
                        }
                        index++;
                    }
                else
                    index += num_bytes_to_skip;
            }
        }
    }
}

inline ustring::ustring( const value_type* str ) :
        ustring()
{
    if( str && str[0] )
    {
        size_type	num_multibytes;
        size_type	nextIndexPosition = 0;
        size_type	string_len = 0;
        size_type	num_bytes = get_num_required_bytes( str , num_multibytes );

        // Initialize the internal buffer
        this->buffer_len = num_bytes + 1; // +1 for the trailling zero
        this->buffer = new char[this->buffer_len];
        this->indices_len = num_multibytes;
        this->indices_of_multibyte = this->indices_len ? new size_type[this->indices_len] : nullptr;

        char* cur_writer = this->buffer;

        // Iterate through wide char literal
        for( size_type i = 0; str[i] ; i++ )
        {
            // Encode wide char to utf8
            unsigned char num_bytes = encode_utf8( str[i] , cur_writer );

            // Push position of character to 'indices_of_multibyte'
            if( num_bytes > 1 )
                this->indices_of_multibyte[nextIndexPosition++] = cur_writer - this->buffer;

            // Step forward with copying to internal utf8 buffer
            cur_writer += num_bytes;

            // Increase string length by 1
            string_len++;
        }

        this->string_len = string_len;

        // Add trailling \0 char to utf8 buffer
        *cur_writer = 0;
    }
}




inline ustring::ustring( ustring&& str ) :
        buffer( str.buffer )
        , buffer_len( str.buffer_len )
        , string_len( str.string_len )
        , indices_of_multibyte( str.indices_of_multibyte )
        , indices_len( str.indices_len )
        , misformatted( str.misformatted )
        , static_buffer( str.static_buffer )
{
    // Reset old string
    str.buffer = nullptr;
    str.indices_of_multibyte = nullptr;
    str.indices_len = 0;
    str.buffer_len = 0;
    str.string_len = 0;
}

inline ustring::ustring( const ustring& str ) :
        buffer( str.empty() ? nullptr : new char[str.buffer_len] )
        , buffer_len( str.buffer_len )
        , string_len( str.string_len )
        , indices_of_multibyte( str.indices_len ? new size_type[str.indices_len] : nullptr )
        , indices_len( str.indices_len )
        , misformatted( str.misformatted )
        , static_buffer( false )
{
    // Clone data_
    if( !str.empty() )
        std::memcpy( this->buffer , str.buffer , str.buffer_len );

    // Clone indices
    if( str.indices_len )
        std::memcpy( this->indices_of_multibyte , str.indices_of_multibyte , str.indices_len * sizeof(size_type) );
}




inline ustring& ustring::operator=( const ustring& str )
{
    if( str.empty() ){
        clear();
        return *this;
    }

    char*		new_buffer = new char[str.buffer_len];
    size_type*	new_indices = str.indices_len ? new size_type[str.indices_len] : nullptr;

    // Clone data_
    std::memcpy( new_buffer , str.buffer , str.buffer_len );

    // Clone indices
    if( str.indices_len > 0 )
        std::memcpy( new_indices , str.indices_of_multibyte , str.indices_len * sizeof(size_type) );

    this->misformatted = str.misformatted;
    this->string_len = str.string_len;
    reset_buffer( new_buffer , str.buffer_len );
    reset_indices( new_indices , str.indices_len );
    return *this;
}

inline ustring& ustring::operator=( ustring&& str )
{
    // Copy data_
    reset_buffer( str.buffer , str.buffer_len );
    reset_indices( str.indices_of_multibyte , str.indices_len );
    this->misformatted = str.misformatted;
    this->string_len = str.string_len;
    this->static_buffer = str.static_buffer;

    // Reset old string
    str.indices_of_multibyte = nullptr;
    str.indices_len = 0;
    str.buffer = nullptr;
    str.buffer_len = 0;
    str.string_len = 0;
    return *this;
}


inline unsigned char ustring::get_num_bytes_of_utf8_char_before( const char* data , size_type current_byte_index ) const
{
    if( current_byte_index >= this->buffer_len || !this->requires_unicode() || current_byte_index < 1 )
        return 1;

    data += current_byte_index;

    // If we know the utf8 string is misformatted, we have to check, how many
    if( this->misformatted )
    {
        // Check if byte is ascii
        if( static_cast<unsigned char>(data[-1]) <= 0x7F )
            return 1;

        // Check if byte is no data_-blob
        if( ( static_cast<unsigned char>(data[-1]) & 0xC0) != 0x80 || current_byte_index < 2 )	return 1;
        // 110XXXXX - two bytes?
        if( ( static_cast<unsigned char>(data[-2]) & 0xE0) == 0xC0 )	return 2;

        // Check if byte is no data_-blob
        if( ( static_cast<unsigned char>(data[-2]) & 0xC0) != 0x80 || current_byte_index < 3 )	return 1;
        // 1110XXXX - three bytes?
        if( ( static_cast<unsigned char>(data[-3]) & 0xF0) == 0xE0 )	return 3;

        // Check if byte is no data_-blob
        if( ( static_cast<unsigned char>(data[-3]) & 0xC0) != 0x80 || current_byte_index < 4 )	return 1;
        // 11110XXX - four bytes?
        if( ( static_cast<unsigned char>(data[-4]) & 0xF8) == 0xF0 )	return 4;

        // Check if byte is no data_-blob
        if( ( static_cast<unsigned char>(data[-4]) & 0xC0) != 0x80 || current_byte_index < 5 )	return 1;
        // 111110XX - five bytes?
        if( ( static_cast<unsigned char>(data[-5]) & 0xFC) == 0xF8 )	return 5;

        // Check if byte is no data_-blob
        if( ( static_cast<unsigned char>(data[-5]) & 0xC0) != 0x80 || current_byte_index < 6 )	return 1;
        // 1111110X - six bytes?
        if( ( static_cast<unsigned char>(data[-6]) & 0xFE) == 0xFC )	return 6;
    }
    else
    {
        // Only Check the possibilities, that could appear
        switch( current_byte_index )
        {
            default:
                if( ( static_cast<unsigned char>(data[-6]) & 0xFE) == 0xFC )  // 1111110X  six bytes
                    return 6;
            case 5:
                if( ( static_cast<unsigned char>(data[-5]) & 0xFC) == 0xF8 )  // 111110XX  five bytes
                    return 5;
            case 4:
                if( ( static_cast<unsigned char>(data[-4]) & 0xF8) == 0xF0 )  // 11110XXX  four bytes
                    return 4;
            case 3:
                if( ( static_cast<unsigned char>(data[-3]) & 0xF0) == 0xE0 )  // 1110XXXX  three bytes
                    return 3;
            case 2:
                if( ( static_cast<unsigned char>(data[-2]) & 0xE0) == 0xC0 )  // 110XXXXX  two bytes
                    return 2;
            case 1:
                break;
        }
    }
    return 1;
}



inline unsigned char ustring::get_num_bytes_of_utf8_char( const char* data , size_type first_byte_index ) const
{
    if( this->buffer_len <= first_byte_index || !this->requires_unicode() )
        return 1;

    data += first_byte_index;
    unsigned char first_byte =  static_cast<unsigned char>(data[0]);

    // If we know the utf8 string is misformatted, we have to check, how many
    if( this->misformatted )
    {
        if( first_byte <= 0x7F )
            return 1;

        // Check if byte is data_-blob
        if( ( static_cast<unsigned char>(data[1]) & 0xC0) != 0x80 )		return 1;
        // 110XXXXX - two bytes?
        if( (first_byte & 0xE0) == 0xC0 )	return 2;

        // Check if byte is data_-blob
        if( ( static_cast<unsigned char>(data[2]) & 0xC0) != 0x80 )		return 1;
        // 1110XXXX - three bytes?
        if( (first_byte & 0xF0) == 0xE0 )	return 3;

        // Check if byte is data_-blob
        if( ( static_cast<unsigned char>(data[3]) & 0xC0) != 0x80 )		return 1;
        // 11110XXX - four bytes?
        if( (first_byte & 0xF8) == 0xF0 )	return 4;

        // Check if byte is data_-blob
        if( ( static_cast<unsigned char>(data[4]) & 0xC0) != 0x80 )		return 1;
        // 111110XX - five bytes?
        if( (first_byte & 0xFC) == 0xF8 )	return 5;

        // Check if byte is data_-blob
        if( ( static_cast<unsigned char>(data[5]) & 0xC0) != 0x80 )		return 1;
        // 1111110X - six bytes?
        if( (first_byte & 0xFE) == 0xFC )	return 6;
    }
    else
    {
        // Only Check the possibilities, that could appear
        switch( this->buffer_len - first_byte_index )
        {
            default:
                if( (first_byte & 0xFE) == 0xFC )  // 1111110X  six bytes
                    return 6;
            case 6:
                if( (first_byte & 0xFC) == 0xF8 )  // 111110XX  five bytes
                    return 5;
            case 5:
                if( (first_byte & 0xF8) == 0xF0 )  // 11110XXX  four bytes
                    return 4;
            case 4:
                if( (first_byte & 0xF0) == 0xE0 )  // 1110XXXX  three bytes
                    return 3;
            case 3:
                if( (first_byte & 0xE0) == 0xC0 )  // 110XXXXX  two bytes
                    return 2;
            case 2:
                if( first_byte <= 0x7F )
                    return 1;
                this->misformatted = true;
            case 1:
            case 0:
                break;
        }
    }
    return 1; // one byte
}



inline unsigned char ustring::decode_utf8( const char* data , value_type& dest ) const
{
    unsigned char first_char =  static_cast<unsigned char>(*data);

    if( !first_char ){
        dest = 0;
        return 1;
    }

    value_type		codepoint;
    unsigned char	num_bytes;

    if( first_char <= 0x7F ){ // 0XXX XXXX one byte
        dest = first_char;
        return 1;
    }
    else if( (first_char & 0xE0) == 0xC0 ){  // 110X XXXX  two bytes
        codepoint = first_char & 31;
        num_bytes = 2;
    }
    else if( (first_char & 0xF0) == 0xE0 ){  // 1110 XXXX  three bytes
        codepoint = first_char & 15;
        num_bytes = 3;
    }
    else if( (first_char & 0xF8) == 0xF0 ){  // 1111 0XXX  four bytes
        codepoint = first_char & 7;
        num_bytes = 4;
    }
    else if( (first_char & 0xFC) == 0xF8 ){  // 1111 10XX  five bytes
        codepoint = first_char & 3;
        num_bytes = 5;
        misformatted = true;
    }
    else if( (first_char & 0xFE) == 0xFC ){  // 1111 110X  six bytes
        codepoint = first_char & 1;
        num_bytes = 6;
    }
    else{ // not a valid first byte of a UTF-8 sequence
        codepoint = first_char;
        num_bytes = 1;
    }

    if( !misformatted )
        for( int i = 1 ; i < num_bytes ; i++ )
            codepoint = (codepoint << 6) | (data[i] & 0x3F);
    else
        for( int i = 1 ; i < num_bytes ; i++ ){
            if( !data[i] || ( static_cast<unsigned char>(data[i]) & 0xC0) != 0x80 ){
                num_bytes = 1;
                codepoint = first_char;
                break;
            }
            codepoint = (codepoint << 6) | ( static_cast<unsigned char>(data[i]) & 0x3F );
        }

    dest = codepoint;

    return num_bytes;
}



inline std::string ustring::cpp_str_bom() const
{
    char bom[4] = { char(0xEF) , char(0xBB) , char(0xBF) , 0 };

    if( !this->buffer )
        return std::string( bom );

    char* tmp_buffer = new char[this->buffer_len + 3];

    tmp_buffer[0] = bom[0];
    tmp_buffer[1] = bom[1];
    tmp_buffer[2] = bom[2];

    std::memcpy( tmp_buffer + 3 , this->buffer , this->buffer_len );

    std::string result = std::string( tmp_buffer );

    delete[] tmp_buffer;

    return move(result);
}


inline ustring::size_type ustring::get_num_required_bytes( const value_type* lit , size_type& num_multibytes )
{
    size_type num_bytes = 0;
    num_multibytes = 0;
    for( size_type i = 0 ; lit[i] ; i++ ){
        unsigned char cur_bytes = get_num_bytes_of_utf8_codepoint( lit[i] );
        if( cur_bytes > 1 )
            num_multibytes++;
        num_bytes += cur_bytes;
    }
    return num_bytes;
}

inline ustring::size_type ustring::get_num_resulting_codepoints( size_type byte_start , size_type byte_count ) const
{
    if( empty() )
        return 0;

    size_type	cur_index = byte_start;
    size_type	codepoint_count = 0;

    byte_count = std::min<difference_type>( byte_count , size() - byte_start );

    while( byte_count > 0 ){
        unsigned char num_bytes = get_num_bytes_of_utf8_char( this->buffer , cur_index );
        cur_index += num_bytes;
        byte_count -= num_bytes;
        codepoint_count++;
    }
    return codepoint_count;
}

inline ustring::size_type ustring::get_num_resulting_bytes( size_type byte_start , size_type codepoint_count ) const
{
    if( empty() )
        return 0;

    if( !requires_unicode() )
        return std::min<size_type>( byte_start + codepoint_count , size() ) - byte_start;

    size_type cur_byte = byte_start;

    while( cur_byte < size() && codepoint_count-- > 0 )
        cur_byte += get_index_bytes( cur_byte );

    return cur_byte - byte_start;
}




inline unsigned char ustring::encode_utf8( value_type codepoint , char* dest )
{
    if( codepoint <= 0x7F ){ // 0XXXXXXX one byte
        dest[0] = char(codepoint);
        return 1;
    }
    if( codepoint <= 0x7FF ){ // 110XXXXX  two bytes
        dest[0] = char( 0xC0 | (codepoint >> 6) );
        dest[1] = char( 0x80 | (codepoint & 0x3F) );
        return 2;
    }
    if( codepoint <= 0xFFFF ){ // 1110XXXX  three bytes
        dest[0] = char( 0xE0 | (codepoint >> 12) );
        dest[1] = char( 0x80 | ((codepoint >> 6) & 0x3F) );
        dest[2] = char( 0x80 | (codepoint & 0x3F) );
        return 3;
    }
    if( codepoint <= 0x1FFFFF ){ // 11110XXX  four bytes
        dest[0] = char( 0xF0 | (codepoint >> 18) );
        dest[1] = char( 0x80 | ((codepoint >> 12) & 0x3F) );
        dest[2] = char( 0x80 | ((codepoint >> 6) & 0x3F) );
        dest[3] = char( 0x80 | (codepoint & 0x3F) );
        return 4;
    }
    if( codepoint <= 0x3FFFFFF ){ // 111110XX  five bytes
        dest[0] = char( 0xF8 | (codepoint >> 24) );
        dest[1] = char( 0x80 | ((codepoint >> 18) & 0x3F) );
        dest[2] = char( 0x80 | ((codepoint >> 12) & 0x3F) );
        dest[3] = char( 0x80 | ((codepoint >> 6) & 0x3F) );
        dest[4] = char( 0x80 | (codepoint & 0x3F) );
        return 5;
    }
    if( codepoint <= 0x7FFFFFFF ){ // 1111110X  six bytes
        dest[0] = char( 0xFC | (codepoint >> 30) );
        dest[1] = char( 0x80 | ((codepoint >> 24) & 0x3F) );
        dest[2] = char( 0x80 | ((codepoint >> 18) & 0x3F) );
        dest[3] = char( 0x80 | ((codepoint >> 12) & 0x3F) );
        dest[4] = char( 0x80 | ((codepoint >> 6) & 0x3F) );
        dest[5] = char( 0x80 | (codepoint & 0x3F) );
        return 6;
    }

    return 1;
}




inline ustring::size_type ustring::get_actual_index( size_type requested_index ) const
{
    if( requested_index >= this->string_len )
        return this->buffer_len - 1;

    size_type index_multibyte_table = 0;
    size_type current_overhead = 0;

    while( index_multibyte_table < this->indices_len )
    {
        size_type multibyte_pos = this->indices_of_multibyte[index_multibyte_table];

        if( multibyte_pos >= requested_index + current_overhead )
            break;

        index_multibyte_table++;
        current_overhead += get_num_bytes_of_utf8_char( this->buffer , multibyte_pos ) - 1; // Ad bytes of multibyte to overhead
    }

    return requested_index + current_overhead;
}

inline ustring ustring::raw_substr( size_type byte_index , size_type tmp_byte_count , size_type num_codepoints ) const
{
    if( byte_index > size() )
        return ustring();

    difference_type byte_count;
    if( tmp_byte_count < size() - byte_index )
        byte_count = tmp_byte_count;
    else
        byte_count = size() - byte_index;

    size_type	actual_start_index = byte_index;
    size_type	actual_end_index = byte_index + byte_count;

    //
    // Manage indices
    //
    size_type	start_of_multibytes_in_range;

    // Look for start of indices
    size_type tmp = 0;
    while( tmp < this->indices_len && this->indices_of_multibyte[tmp] < actual_start_index )
        tmp++;
    start_of_multibytes_in_range = tmp;

    // Look for the end
    while( tmp < this->indices_len && this->indices_of_multibyte[tmp] < actual_end_index )
        tmp++;

    // Compute number of indices
    size_type	indices_len = tmp - start_of_multibytes_in_range;
    size_type*	new_indices = indices_len ? new size_type[indices_len] : nullptr;

    // Copy indices
    for( size_type i = 0 ; i < indices_len ; i++ )
        new_indices[i] = this->indices_of_multibyte[start_of_multibytes_in_range + i] - actual_start_index;
    //
    // Manage utf8 string
    //

    // Allocate new buffer
    char*	new_buffer = new char[byte_count+1];

    // Partly copy
    std::memcpy( new_buffer , this->buffer + actual_start_index , byte_count );

    new_buffer[byte_count] = 0;
    byte_count++;

    return ustring( new_buffer , byte_count , num_codepoints , new_indices , indices_len );
}


inline void ustring::raw_replace( size_type actual_start_index , size_type replaced_bytes , const ustring& replacement )
{
    if( actual_start_index > size() )
        actual_start_index = size();

    size_type		replacement_bytes = replacement.size();
    size_type		replacement_indices = replacement.indices_len;
    difference_type	byte_difference = replaced_bytes - replacement_bytes;
    size_type		actual_end_index;

    if( replaced_bytes < size() - actual_start_index )
        actual_end_index = actual_start_index + replaced_bytes;
    else{
        actual_end_index = size();
        replaced_bytes = actual_end_index - actual_start_index;
    }

    //
    // Manage indices
    //

    size_type	start_of_multibytes_in_range;
    size_type	number_of_multibytes_in_range;

    // Look for start of indices
    size_type tmp = 0;
    while( tmp < this->indices_len && this->indices_of_multibyte[tmp] < actual_start_index )
        tmp++;
    start_of_multibytes_in_range = tmp;

    // Look for the end
    while( tmp < this->indices_len && this->indices_of_multibyte[tmp] < actual_end_index )
        tmp++;

    // Compute number of indices
    number_of_multibytes_in_range = tmp - start_of_multibytes_in_range;

    // Compute difference in number of indices
    difference_type indices_difference = number_of_multibytes_in_range - replacement_indices;

    // Create new buffer
    size_type indices_len = this->indices_len - indices_difference;

    if( indices_len )
    {
        size_type*	new_indices = indices_len ? new size_type[ indices_len ] : nullptr;

        // Copy values before replacement start
        for( size_type i = 0 ; i < this->indices_len && this->indices_of_multibyte[i] < actual_start_index ; i++ )
            new_indices[i] = this->indices_of_multibyte[i];

        // Copy the values after the replacement
        if( indices_difference != 0 ){
            for(
                    size_type i = start_of_multibytes_in_range
                    ; i + number_of_multibytes_in_range < this->indices_len
                    ; i++
                    )
                new_indices[ i + replacement_indices ] = this->indices_of_multibyte[ i + number_of_multibytes_in_range ] - byte_difference;
        }
        else{
            size_type i = start_of_multibytes_in_range + number_of_multibytes_in_range;
            while( i < this->indices_len ){
                new_indices[i] = this->indices_of_multibyte[i] - byte_difference;
                i++;
            }
        }

        // Insert indices from replacement
        for( size_type i = 0 ; i < replacement_indices ; i++ )
            new_indices[ start_of_multibytes_in_range + i ] = replacement.indices_of_multibyte[i] + actual_start_index;

        // Move and reset
        reset_indices( new_indices , indices_len );
    }
    else
        // Reset because empty
        reset_indices();

    //
    // Manage utf8 data_
    //

    // Allocate new buffer
    size_type normalized_buffer_len = std::max<size_type>( this->buffer_len , 1 );
    size_type new_buffer_len = normalized_buffer_len + ( replacement_bytes - replaced_bytes );

    if( new_buffer_len > 1 )
    {
        char* new_buffer = new char[new_buffer_len];

        // Partly copy
        // Copy string until replacement index
        std::memcpy( new_buffer , this->buffer , actual_start_index );

        // Copy rest
        std::memcpy(
                new_buffer + actual_start_index + replacement_bytes
                , this->buffer + actual_end_index
                , normalized_buffer_len - actual_end_index - 1 // ('-1' for skipping the trailling zero which is not present in case the buffer_len was 0)
        );

        // Set trailling zero
        new_buffer[ new_buffer_len - 1 ] = '\0';

        // Write bytes
        std::memcpy( new_buffer + actual_start_index , replacement.buffer , replacement_bytes );

        // Adjust string length
        this->string_len -= get_num_resulting_codepoints( actual_start_index , replaced_bytes );
        this->string_len += replacement.length();

        // Rewrite buffer
        reset_buffer( new_buffer , new_buffer_len );

        this->misformatted = this->misformatted || replacement.misformatted;
    }
    else
        // Reset because empty
        clear();
}




inline ustring::value_type ustring::at( size_type requested_index ) const
{
    if( requested_index >= size() )
        return 0;

    if( !requires_unicode() )
        return (value_type) this->buffer[requested_index];

    // Decode internal buffer at position n
    value_type codepoint = 0;
    decode_utf8( this->buffer + get_actual_index( requested_index ) , codepoint );

    return codepoint;
}




inline std::unique_ptr<ustring::value_type[]> ustring::wide_literal() const
{
    if( empty() )
        return std::unique_ptr<value_type[]>( new value_type[1]{0} );

    std::unique_ptr<value_type[]>	dest = std::unique_ptr<value_type[]>( new value_type[length()] );
    value_type*						tempDest = dest.get();
    char*							source = this->buffer;

    while( *source )
        source += decode_utf8( source , *tempDest++ );

    return std::move(dest);
}




inline ustring::difference_type ustring::compare( const ustring& str ) const
{
    const_iterator	it1 = cbegin();
    const_iterator	it2 = str.cbegin();
    const_iterator	end1 = cend();
    const_iterator	end2 = str.cend();

    while( it1 != end1 && it2 != end2 ){
        difference_type diff = *it2 - *it1;
        if( diff )
            return diff;
        it1++;
        it2++;
    }
    return ( it1 == end1 ? 1 : 0 ) - ( it2 == end2 ? 1 : 0 );
}

inline bool ustring::equals( const char* str ) const
{
    const char* it1 = this->buffer;

    if( !it1 || !str )
        return (long)it1 == *str;

    while( *it1 && *str ){
        if( *it1 != *str )
            return false;
        it1++;
        str++;
    }
    return *it1 == *str;
}

inline bool ustring::equals( const value_type* str ) const
{
    const_iterator	it = cbegin();
    const_iterator	end = cend();

    while( it != end && *str ){
        if( *str != *it )
            return false;
        it++;
        str++;
    }
    return *it == *str;
}




inline ustring::size_type ustring::find( value_type ch , size_type start_pos ) const
{
    for( const_iterator it = get(start_pos) ; it < cend() ; it++ )
        if( *it == ch )
            return it - begin();
    return ustring::npos;
}

inline ustring::size_type ustring::raw_find( value_type ch , size_type byte_start ) const
{
    for( size_type it = byte_start ; it < size() ; it += get_index_bytes( it ) )
        if( raw_at(it) == ch )
            return it;
    return ustring::npos;
}




inline ustring::size_type ustring::rfind( value_type ch , size_type start_pos ) const
{
    const_reverse_iterator it = ( start_pos >= length() ) ? crbegin() : rget( start_pos );
    while( it < crend() ){
        if( *it == ch )
            return -( it - cbegin() );
        it++;
    }
    return ustring::npos;
}

inline ustring::size_type ustring::raw_rfind( value_type ch , size_type byte_start ) const
{
    if( byte_start >= size() )
        byte_start = back_index();

    for( difference_type it = byte_start ; it >= 0 ; it -= get_index_pre_bytes( it ) )
        if( raw_at(it) == ch )
            return it;

    return ustring::npos;
}




inline ustring::size_type ustring::find_first_of( const value_type* str , size_type start_pos ) const
{
    if( start_pos >= length() )
        return ustring::npos;

    for( const_iterator it = get( start_pos ) ; it < cend() ; it++ )
    {
        const value_type*	tmp = str;
        value_type			cur = *it;
        do{
            if( cur == *tmp )
                return it - begin();
        }while( *++tmp );
    }

    return ustring::npos;
}

inline ustring::size_type ustring::raw_find_first_of( const value_type* str , size_type byte_start ) const
{
    if( byte_start >= size() )
        return ustring::npos;

    for( size_type it = byte_start ; it < size() ; it += get_index_bytes( it ) )
    {
        const value_type*	tmp = str;
        do{
            if( raw_at(it) == *tmp )
                return it;
        }while( *++tmp );
    }

    return ustring::npos;
}




inline ustring::size_type ustring::find_last_of( const value_type* str , size_type start_pos ) const
{
    const_reverse_iterator it = ( start_pos >= length() ) ? crbegin() : rget( start_pos );

    while( it < rend() )
    {
        const value_type*	tmp = str;
        value_type			cur = *it;
        do{
            if( cur == *tmp )
                return -( it - begin() );
        }while( *++tmp );
        it++;
    }

    return ustring::npos;
}

inline ustring::size_type ustring::raw_find_last_of( const value_type* str , size_type byte_start ) const
{
    if( empty() )
        return ustring::npos;

    if( byte_start >= size() )
        byte_start = back_index();

    for( difference_type it = byte_start ; it >= 0 ; it -= get_index_pre_bytes( it ) )
    {
        const value_type*	tmp = str;
        value_type			cur = raw_at(it);
        do{
            if( cur == *tmp )
                return it;
        }while( *++tmp );
    }

    return ustring::npos;
}




inline ustring::size_type ustring::find_first_not_of( const value_type* str , size_type start_pos ) const
{
    if( start_pos >= length() )
        return ustring::npos;

    for( const_iterator it = get(start_pos) ; it < cend() ; it++ )
    {
        const value_type*	tmp = str;
        value_type			cur = *it;
        do{
            if( cur == *tmp )
                goto continue2;
        }while( *++tmp );

        return it - begin();

        continue2:;
    }

    return ustring::npos;
}

inline ustring::size_type ustring::raw_find_first_not_of( const value_type* str , size_type byte_start ) const
{
    if( byte_start >= size() )
        return ustring::npos;

    for( size_type it = byte_start ; it < size() ; it += get_index_bytes( it ) )
    {
        const value_type*	tmp = str;
        value_type			cur = raw_at(it);
        do{
            if( cur == *tmp )
                goto continue2;
        }while( *++tmp );

        return it;

        continue2:;
    }

    return ustring::npos;
}




inline ustring::size_type ustring::find_last_not_of( const value_type* str , size_type start_pos ) const
{
    if( empty() )
        return ustring::npos;

    const_reverse_iterator it = ( start_pos >= length() ) ? crbegin() : rget( start_pos );

    while( it < rend() )
    {
        const value_type*	tmp = str;
        value_type			cur = *it;
        do{
            if( cur == *tmp )
                goto continue2;
        }while( *++tmp );

        return -( it - begin() );

        continue2:
        it++;
    }

    return ustring::npos;
}

inline ustring::size_type ustring::raw_find_last_not_of( const value_type* str , size_type byte_start ) const
{
    if( empty() )
        return ustring::npos;

    if( byte_start >= size() )
        byte_start = back_index();

    for( difference_type it = byte_start ; it >= 0 ; it -= get_index_pre_bytes( it ) )
    {
        const value_type*	tmp = str;
        value_type			cur = raw_at(it);

        do{
            if( cur == *tmp )
                goto continue2;
        }while( *++tmp );

        return it;

        continue2:;
    }

    return ustring::npos;
}


inline int operator-( const ustring::const_iterator& lhs , const ustring::const_iterator& rhs )
{
    ustring::difference_type minIndex = std::min( lhs.get_index() , rhs.get_index() );
    ustring::difference_type max_index = std::max( lhs.get_index() , rhs.get_index() );
    ustring::size_type num_codepoints = lhs.get_instance()->get_num_resulting_codepoints( minIndex , max_index - minIndex );
    return max_index == lhs.get_index() ? num_codepoints : -num_codepoints;
}

inline int operator-( const ustring::const_reverse_iterator& lhs , const ustring::const_reverse_iterator& rhs )
{
    ustring::difference_type	minIndex = std::min( lhs.get_index() , rhs.get_index() );
    ustring::difference_type	max_index = std::max( lhs.get_index() , rhs.get_index() );
    ustring::size_type			num_codepoints = lhs.get_instance()->get_num_resulting_codepoints( minIndex , max_index - minIndex );
    return max_index == rhs.get_index() ? num_codepoints : -num_codepoints;
}

#endif //USTRING_H
