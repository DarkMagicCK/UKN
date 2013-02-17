
// safe bool idiom
// for safer bool conversion operator before C++11
// though explicit conversion operator is in the C++11 standard
// not every compiler supports it (such as vc++11)
// see www.artima.com/cppsource/safebool3.html

namespace mist {

    namespace detail {
     
        class safe_bool_base {
        public:
            void this_type_does_not_support_comparisions() const { }

        protected:
            typedef void(safe_bool_base::*bool_type)() const;
            
            safe_bool_base() {}
            safe_bool_base(const safe_bool_base&) {}
            safe_bool_base& operator=(const safe_bool_base&) { return *this; }
            ~safe_bool_base() {} 
        };
    
    }
    
    template<typename T = void>
    class safe_bool: public detail::safe_bool_base {
    public:
        operator bool_type() const {
            return (static_cast<const T*>(this))->boolean_test() ?
                &safe_bool_base::this_type_does_not_support_comparisions :
                0;
        }
            
    protected:
        ~safe_bool() {}
    };

    template<>
    class safe_bool<void>: public detail::safe_bool_base {
    public:
        operator bool_type() const {
            return boolean_test() == true ?
                &safe_bool_base::this_type_does_not_support_comparisions :
                0;
        }
            
    protected:
        virtual bool boolean_test() const = 0;
        ~safe_bool() {}
    };

    template<typename T, typename U>
    bool operator==(const safe_bool<T>& lhs, const safe_bool<U>& rhs) {
        lhs.this_type_does_not_support_comparisions();
        return false;
    }

    template<typename T, typename U>
    bool operator!=(const safe_bool<T>& lhs, const safe_bool<U>& rhs) {
        lhs.this_type_does_not_support_comparisions();
        return false;
    }

} // namespace mist