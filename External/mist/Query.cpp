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
            
            std::vector<int> t = From(v)
                                    .Where([](const int& i) {
                                        return i > 4;
                                    })
                                    .OrderBy(mist::query::Descending);
            
            mist_assert_l(t.size() == 5, "invalid size");
            std::copy(t.begin(), t.end(),
                      std::ostream_iterator<int>(std::cout, ", "));
            std::cout<<std::endl;
            
            int arr[10] = { 23, 1, 23, 6, 2, 3, 6, 0, 2, 1 };
            t = From(arr)
                    .Where([](const int& i) {
                        return i < 5;
                    })
                    .OrderBy(mist::query::Descending);
            
            mist_assert_l(t.size() == 6, "invalid size");
            std::copy(t.begin(), t.end(),
                      std::ostream_iterator<int>(std::cout, ", "));
            std::cout<<std::endl;
            
            struct Cat {
                std::string name;
                std::string moew;
                int id;
                Cat() { }
                Cat(const std::string& name, const std::string& moew, int id):
                name(name), moew(moew), id(id) { }
            };
            
            std::vector<Cat> cat = {
                Cat("1", "m", 1),
                Cat("2", "abcc", 2),
                Cat("3", "ddddd", 3),
                Cat("2", "hsdsd", 4),
                Cat("5", "hsdsd", 5)
            };
            auto c = From(cat)
                        .Where([](const Cat& c) { return c.name == "1"; });
            mist_assert_l(c.size() == 1, "invalid size");
            std::cout<<c[0].name<<"\t"<<c[0].moew<<std::endl;
            
            auto c2 = From(cat)
                        .GroupBy<std::string>([](const Cat& c) { return c.name; });
            
            auto enumerator = c2.createEnumerator();
            while(enumerator->available()) {
                std::cout<<enumerator->current().first<<std::endl;
                
                std::vector<Cat> s = enumerator->current().second;
                for(Cat cat: s) {
                    std::cout << "\t" << cat.name << "\t" << cat.moew;
                    std::cout << std::endl;
                }
                
                enumerator->next();
            }

            
            std::vector<std::string> moews = {
                "m",
                "aaaddd",
                "m",
                "hsdsd",
                "m",
                "hsdsd",
            };
            
            /* inner join */
            auto c4 = From(cat)
                        .Where([](const Cat& c) { return c.id >= 2; })
                        .JoinOn(moews, [](const Cat& cat, const std::string& moew) {
                            return cat.moew == moew;
                        })
                        .Select<std::string>([](const std::pair<Cat, std::string>& p) {
                            return p.first.name;
                        });
            
            std::cout << "*** Inner Join ***" << std::endl;

            for(decltype(c4)::element_type e: c4) {
                std::cout << e << std::endl;
            }
            
            /* group left outer join */
            auto c5 = From(moews)
                        .JoinOnGroup(cat,
                                    [](const std::string& moew, const Cat& cat) {
                                        return cat.moew == moew;
                                    },
                                    []() {
                                        return Cat("-1", "SchrodingerCat", -1);
                                    });
            std::cout << "*** Group Inner Join ***" << std::endl;

            for(decltype(c5)::element_type e: c5) {
                std::cout << "Group: "<< e.first << std::endl;
                for(decltype(e.second)::const_iterator it = e.second.begin(), end = e.second.end();
                    it != end;
                    ++it) {
                    std::cout << "\t" << it->name << std::endl;
                }
            }
        }
        
    }
    
}