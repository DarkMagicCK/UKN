//
//  Query.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/10/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "Query.h"
#include "Platform.h"

#include <vector>
#include <list>
#include <map>

#include <iostream>
#include <iterator>

#include "Logger.h"

namespace mist {
    
    namespace query {
        
        void _TestQuery() {
            std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
            
            std::vector<int> t = Select(v)
                                    .Where([](const int& i) {
                                        return i > 4;
                                    })
                                    .Orderby(mist::query::Descending);
            
            mist_assert_l(t.size() == 5, "invalid size");
            std::copy(t.begin(), t.end(),
                      std::ostream_iterator<int>(std::cout, ", "));
            std::cout<<std::endl;
            
            int arr[10] = { 23, 1, 23, 6, 2, 3, 6, 0, 2, 1 };
            t = Select(arr)
                    .Where([](const int& i) {
                        return i < 5;
                    })
                    .Orderby(mist::query::Descending);
            
            mist_assert_l(t.size() == 6, "invalid size");
            std::copy(t.begin(), t.end(),
                      std::ostream_iterator<int>(std::cout, ", "));
            std::cout<<std::endl;
            
            struct Cat {
                std::string name;
                std::string moew;
                Cat() { }
                Cat(const std::string& name, const std::string& moew):
                name(name), moew(moew) { }
            };
            
            std::vector<Cat> cat = {
                Cat("1", "m"),
                Cat("2", "abcc"),
                Cat("3", "ddddd"),
                Cat("4", "hsdsd")
            };
            auto c = Select(cat)
                        .Where([](const Cat& c) { return c.name == "1"; });
            mist_assert_l(c.size() == 1, "invalid size");
            std::cout<<c[0].name<<"\t"<<c[0].moew<<std::endl;
            
            c = Select(cat)
                    .Orderby([](const Cat& c) { return c.moew; });
            mist_assert_l(c.size() == 4, "invalid size");
            for(Cat cat: c) {
                std::cout<<cat.name<<"\t"<<cat.moew<<std::endl;
            }
        }
        
    }
    
}