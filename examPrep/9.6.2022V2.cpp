#include <iostream>
#include <cassert>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <unordered_map>
#include <memory>
#include <unordered_set>
#include <queue>
#include <list>
#include <sstream>
#include <cstring>

using namespace std;

class CPkg
{
    using datatype = set<string>;

public:
    CPkg() = default;
    ~CPkg() = default;

    CPkg(const string &str) : m_Name(std::move(str)) {}

    CPkg &addDep(const string &dep)
    {
        m_Dep.emplace(std::move(dep));
        return *this;
    }

    bool operator<(const CPkg &rhs) const
    {
        return m_Name < rhs.m_Name;
    }

    pair<datatype::const_iterator, datatype::const_iterator> getDep() const
    {
        return {m_Dep.begin(), m_Dep.end()};
    }

    string toString() const
    {
        return m_Name;
    }

private:
    string m_Name;
    set<string> m_Dep;
};
class CPkgSys
{
public:
    CPkgSys &addPkg(const CPkg &package)
    {
        m_System.emplace(std::move(package), false);
        return *this;
    }

    set<string> install(const list<string> &list)
    {
        set<string> result;
        map<CPkg, bool> temp = m_System;

        for (const auto &pkg : list)
            installDep(pkg, temp, result);

        swap(temp, m_System);
        return result;
    }

    friend ostream &operator<<(ostream &os, const CPkgSys &src)
    {
        bool first = true;
        for (const auto &[package, installed] : src.m_System)
        {
            if (installed)
            {
                if (!first)
                    os << ", ";
                else
                    first = false;
                os << package.toString();
            }
        }
        return os;
    }

private:
    map<CPkg, bool> m_System; // false => not installed

    static void installDep(const string &pkgName, map<CPkg, bool> &system, set<string> &result)
    {
        queue<string> queue({pkgName});
        while (!queue.empty())
        {
            string current = queue.front();
            queue.pop();

            auto it = system.find(current);
            if (it == system.end())
                throw std::invalid_argument("Package not found.");

            else if (it->second)
                continue;

            auto [lo, hi] = it->first.getDep();
            for (auto it = lo; it != hi; ++it)
                queue.emplace(*it);

            it->second = true;
            result.emplace(it->first.toString());
        }
    }
};

int main(void)
{
    CPkgSys s;
    stringstream ss;
    s.addPkg(CPkg("ssh").addDep("sudo").addDep("apt")).addPkg(CPkg("sudo").addDep("git").addDep("c++"));
    s.addPkg(CPkg("apt")).addPkg(CPkg("c++").addDep("c").addDep("asm").addDep("fortran"));
    s.addPkg(CPkg("git")).addPkg(CPkg("c").addDep("kekw")).addPkg(CPkg("kekw")).addPkg(CPkg("asm")).addPkg(CPkg("fortran"));
    s.addPkg(CPkg("python").addDep("bash").addDep("sadge")).addPkg(CPkg("karel").addDep("python")).addPkg(CPkg("bash").addDep("sadge")).addPkg(CPkg("sadge")).addPkg(CPkg("cython").addDep("dev"));
    s.addPkg(CPkg("perl"));

    ss << s;
    assert(ss.str() == "");
    ss.clear();
    ss.str("");

    set<string> t1 = s.install(list<string>{"sudo"});
    assert(t1 == (set<string>{"asm", "c", "c++", "fortran", "git", "kekw", "sudo"}));
    set<string> t2 = s.install(list<string>{"ssh", "c++"});
    assert(t2 == (set<string>{"apt", "ssh"}));

    ss << s;
    assert(ss.str() == "apt, asm, c, c++, fortran, git, kekw, ssh, sudo");
    ss.clear();
    ss.str("");

    try
    {
        set<string> e = s.install(list<string>{"karel", "cython"});
        assert("Sem ses nemel dostat debilku" == nullptr);
    }
    catch (const invalid_argument &e)
    {
        assert(strcmp("Package not found.", e.what()) == 0);
    }
    set<string> t3 = s.install(list<string>{"karel", "fortran", "git"});
    assert(t3 == (set<string>{"bash", "karel", "python", "sadge"}));

    s.addPkg(CPkg("java").addDep("utils")).addPkg(CPkg("utils").addDep("VB")).addPkg(CPkg("VB").addDep("java"));

    set<string> t4 = s.install(list<string>{"java", "perl"});
    assert(t4 == (set<string>{"VB", "java", "perl", "utils"}));

    ss << s;
    assert(ss.str() == "VB, apt, asm, bash, c, c++, fortran, git, java, karel, kekw, perl, python, sadge, ssh, sudo, utils");
    ss.clear();
    ss.str("");

    CPkgSys k;
    k.addPkg(CPkg("ssh").addDep("sudo").addDep("apt")).addPkg(CPkg("sudo").addDep("git"));
    k.addPkg(CPkg("apt"));
    k.addPkg(CPkg("git")).addPkg(CPkg("c").addDep("kekw")).addPkg(CPkg("kekw")).addPkg(CPkg("asm")).addPkg(CPkg("fortran"));
    k.addPkg(CPkg("python").addDep("bash").addDep("sadge")).addPkg(CPkg("karel").addDep("python")).addPkg(CPkg("bash").addDep("sadge")).addPkg(CPkg("sadge"));
    k.addPkg(CPkg("perl").addDep("no"));

    set<string> t5 = k.install(list<string>{"asm"});
    assert(t5 == (set<string>{"asm"}));
    set<string> t6 = k.install(list<string>{"python", "ssh"});
    assert(t6 == (set<string>{"apt", "bash", "git", "python", "sadge", "ssh", "sudo"}));

    try
    {
        set<string> t7 = k.install(list<string>{"perl", "c"});
        assert("Sem ses nemel dostat debilku" == nullptr);
    }
    catch (const invalid_argument &e)
    {
        assert(strcmp("Package not found.", e.what()) == 0);
    }
    set<string> t8 = k.install(list<string>{"c", "ssh", "karel"});

    assert(t8 == (set<string>{"c", "karel", "kekw"}));

    ss << k;
    assert(ss.str() == "apt, asm, bash, c, git, karel, kekw, python, sadge, ssh, sudo");
    ss.clear();
    ss.str("");
}