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
public:
    CPkg() = default;
    CPkg(const std::string &pkgName) : m_PkgName(std::move(pkgName)) {}
    CPkg &addDep(const std::string &dep)
    {
        m_dps.emplace(std::move(dep));
        return *this;
    }
    std::string toString() const
    {
        return m_PkgName;
    }
    std::pair<std::unordered_set<std::string>::const_iterator, std::unordered_set<std::string>::const_iterator> findRange() const
    {
        return {m_dps.begin(), m_dps.end()};
    }
    bool operator<(const CPkg &rhs) const
    {
        return m_PkgName < rhs.m_PkgName;
    }

private:
    std::string m_PkgName;
    std::unordered_set<std::string> m_dps;
};

class CPkgSys
{
public:
    CPkgSys &addPkg(const CPkg &pkg)
    {
        m_packages.emplace(std::move(pkg), false);
        return *this;
    }

    std::set<std::string> install(const std::list<std::string> &pkgList)
    {
        std::set<std::string> result;
        std::map<CPkg, bool> tmpPkgMap = m_packages;

        for (const auto &pkg : pkgList)
        {
            installPkg(pkg, tmpPkgMap, result);
        }

        swap(tmpPkgMap, m_packages);
        return result;
    }

    friend std::ostream &operator<<(std::ostream &os, const CPkgSys &src)
    {
        bool first = true;
        for (const auto &[pkg, installed] : src.m_packages)
        {
            if (installed)
            {
                if (!first)
                    os << ", ";
                else
                    first = false;
                os << pkg.toString();
            }
        }
        return os;
    }

private:
    std::map<CPkg, bool> m_packages;

    static void installPkg(const std::string &pkgName, std::map<CPkg, bool> &tmpPkgMap, std::set<std::string> &result)
    {
        std::queue<std::string> q({pkgName});
        while (!q.empty())
        {
            std::string current{std::move(q.front())};
            q.pop();

            auto it = tmpPkgMap.find(current);
            if (it == tmpPkgMap.end())
                throw std::invalid_argument("Package not found.");
            else if (it->second)
                continue;

            auto [lo, hi] = it->first.findRange();
            for (auto it = lo; it != hi; ++it)
                q.emplace(std::move(*it));

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