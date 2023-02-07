#pragma once

namespace set {

struct Interface;
using $ = std::unique_ptr<Interface>;

template <typename T> concept derived = (std::derived_from<T, Interface> && !std::is_same_v<T, Interface>);

struct Interface {
    virtual ~Interface() = default;

    virtual Interface const &thisset() const = 0;
    virtual Interface const &superset() const = 0;

    virtual $ operator+(Interface const &set) const = 0;
    virtual $ operator-(Interface const &set) const = 0;
    virtual $ operator*(Interface const &set) const = 0;
    virtual $ operator/(Interface const &set) const = 0;
    virtual $ operator!() const = 0;
    virtual $ operator-() const = 0;

    virtual $ operator<(Interface const &set) const = 0;
    virtual $ operator>(Interface const &set) const = 0;
    virtual $ operator<=(Interface const &set) const = 0;
    virtual $ operator>=(Interface const &set) const = 0;
    virtual $ operator||(Interface const &set) const = 0;
    virtual $ operator&&(Interface const &set) const = 0;

    virtual $ operator==(Interface const &set) const = 0;
    virtual $ operator!=(Interface const &set) const = 0;
    virtual $ contains(Interface const &set) const = 0;
    virtual bool ok() const = 0;

    virtual $ extract(std::string const &name) const = 0;

    virtual $ clone() const = 0;
    virtual std::string show() const = 0;

    template <typename T> requires derived<T> T &cast() { return *static_cast<T *>(this); }
    template <typename T> requires derived<T> T const &cast() const { return *static_cast<T const *>(this); }
};

class Set {
public:
    Set($ &&set) : _set(std::move(set)) {}

    template <typename T> requires derived<T> T const &cast() const { return _set->cast<T>(); }
    template <typename T> requires derived<T> T &cast() { return _set->cast<T>(); }

    template <typename T> requires derived<T> std::unique_ptr<T> move();
    $ move();

    Set operator+(Set const &set) const { return *_set + *set._set; }
    Set operator-(Set const &set) const { return *_set - *set._set; }
    Set operator*(Set const &set) const { return *_set * *set._set; }
    Set operator/(Set const &set) const { return *_set / *set._set; }
    Set operator!() const { return !*_set; }
    Set operator-() const { return -*_set; }
    Set operator<(Set const &set) const { return *_set < *set._set; }
    Set operator>(Set const &set) const { return *_set > *set._set; }
    Set operator<=(Set const &set) const { return *_set <= *set._set; }
    Set operator>=(Set const &set) const { return *_set >= *set._set; }
    Set operator||(Set const &set) const { return *_set || *set._set; }
    Set operator&&(Set const &set) const { return *_set && *set._set; }
    Set operator==(Set const &set) const { return *_set == *set._set; }
    Set operator!=(Set const &set) const { return *_set != *set._set; }
    Set contains(Set const &set) const { return _set->contains(*set._set); }
    bool ok() const { return _set->ok(); }
    Set extract(std::string const &name) const { return _set->extract(name); }
    Set clone() const { return _set->clone(); }
    std::string show() const { return _set->show(); }

private:
    $ _set;
};

struct Identity : Interface {
    Identity() = default;

    Interface const &thisset() const override { return *this; }
    Interface const &superset() const override;

    $ operator+(Interface const & /*set*/) const override;
    $ operator-(Interface const & /*set*/) const override;
    $ operator*(Interface const & /*set*/) const override;
    $ operator/(Interface const & /*set*/) const override;
    $ operator!() const override;
    $ operator-() const override;

    $ operator<(Interface const & /*set*/) const override;
    $ operator>(Interface const & /*set*/) const override;
    $ operator<=(Interface const & /*set*/) const override;
    $ operator>=(Interface const & /*set*/) const override;
    $ operator||(Interface const & /*set*/) const override;
    $ operator&&(Interface const & /*set*/) const override;

    $ operator==(Interface const &set) const override;
    $ operator!=(Interface const &set) const override;
    $ contains(Interface const &set) const override { return thisset() == set.superset(); }
    bool ok() const override { return true; }
    $ extract(std::string const &name) const override;

    $ clone() const override;

    std::string show() const override { return std::format("id: {}", (void *)this); }
};

struct Failure : Interface {
    constexpr static Identity const super{};
    Failure(std::string msg = "failure") : msg(std::move(msg)) {}
    Interface const &thisset() const override { return *this; }
    Interface const &superset() const override { return super; }

    $ operator+(Interface const & /*set*/) const override { return clone(); }
    $ operator-(Interface const & /*set*/) const override { return clone(); }
    $ operator*(Interface const & /*set*/) const override { return clone(); }
    $ operator/(Interface const & /*set*/) const override { return clone(); }
    $ operator!() const override { return clone(); }
    $ operator-() const override { return clone(); }

    $ operator<(Interface const & /*set*/) const override { return clone(); }
    $ operator>(Interface const & /*set*/) const override { return clone(); }
    $ operator<=(Interface const & /*set*/) const override { return clone(); }
    $ operator>=(Interface const & /*set*/) const override { return clone(); }
    $ operator||(Interface const & /*set*/) const override { return clone(); }
    $ operator&&(Interface const & /*set*/) const override { return clone(); }

    $ operator==(Interface const & /*set*/) const override { return clone(); }
    $ operator!=(Interface const & /*set*/) const override { return clone(); }
    $ contains(Interface const & /*set*/) const override { return clone(); }
    bool ok() const override { return false; }
    $ extract(std::string const & /*name*/) const override { return clone(); }

    $ clone() const override { return std::make_unique<Failure>(msg); }
    std::string show() const override { return msg; }

    std::string msg;
};

struct Void : Interface {
    constexpr static Identity const id{};
    Interface const &thisset() const override { return id; } // ref of identity
    Interface const &superset() const override;

    $ operator+(Interface const & /*set*/) const override { return clone(); }
    $ operator-(Interface const & /*set*/) const override { return clone(); }
    $ operator*(Interface const & /*set*/) const override { return clone(); }
    $ operator/(Interface const & /*set*/) const override { return clone(); }
    $ operator!() const override;
    $ operator-() const override { return clone(); }

    $ operator<(Interface const & /*set*/) const override { return clone(); }
    $ operator>(Interface const & /*set*/) const override { return clone(); }
    $ operator<=(Interface const & /*set*/) const override { return clone(); }
    $ operator>=(Interface const & /*set*/) const override { return clone(); }
    $ operator||(Interface const & /*set*/) const override { return clone(); }
    $ operator&&(Interface const & /*set*/) const override { return clone(); }

    $ operator==(Interface const &set) const override { return id == set; }
    $ operator!=(Interface const &set) const override { return id != set; }
    $ contains(Interface const &set) const override { return id == set; }
    bool ok() const override { return true; }
    $ extract(std::string const & /*name*/) const override { return std::make_unique<Failure>(); }

    $ clone() const override { return std::make_unique<Void>(); }
    std::string show() const override { return "void"; }
};

struct Universe : Interface {
    constexpr static Identity const id{};
    Interface const &thisset() const override { return id; }     // ref of identity
    Interface const &superset() const override { return *this; } // !

    $ operator+(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator-(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator*(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator/(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator!() const override { return std::make_unique<Void>(); }
    $ operator-() const override { return std::make_unique<Void>(); }

    $ operator<(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator>(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator<=(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator>=(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator||(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator&&(Interface const & /*set*/) const override { return std::make_unique<Void>(); }

    $ operator==(Interface const &set) const override { return id == set; }
    $ operator!=(Interface const &set) const override { return id != set; }
    $ contains(Interface const & /*set*/) const override;
    bool ok() const override { return true; }
    $ extract(std::string const & /*name*/) const override { return std::make_unique<Failure>(); }

    $ clone() const override { return std::make_unique<Universe>(); }
    std::string show() const override { return "universe"; }
};

template <typename T> class Base : public Interface {
    constexpr static bool isBool = std::is_same_v<bool, T>;

public:
    constexpr static Identity const super{};

    Base(T val) : _val(val) {}

    Interface const &thisset() const override { return *this; }

    Interface const &superset() const override { return super; }

    $ operator+(Interface const &set) const override {
        if constexpr (isBool) {
            return std::make_unique<Void>();
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs + rhs; }>(set);
        }
    }

    $ operator-(Interface const &set) const override {
        if constexpr (isBool) {
            return std::make_unique<Void>();
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs - rhs; }>(set);
        }
    }

    $ operator*(Interface const &set) const override {
        if constexpr (isBool) {
            return std::make_unique<Void>();
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs * rhs; }>(set);
        }
    }

    $ operator/(Interface const &set) const override {
        if constexpr (isBool) {
            return std::make_unique<Void>();
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs / rhs; }>(set);
        }
    }

    $ operator!() const override {
        if constexpr (isBool) {
            return std::make_unique<Base<bool>>(!_val);
        } else {
            return std::make_unique<Void>();
        }
    }

    $ operator-() const override {
        if constexpr (isBool) {
            return std::make_unique<Void>();
        } else {
            return std::make_unique<Base<T>>(-_val);
        }
    }

    $ operator<(Interface const &set) const override {
        if constexpr (isBool) {
            return std::make_unique<Void>();
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs < rhs; }>(set);
        }
    }

    $ operator>(Interface const &set) const override {
        if constexpr (isBool) {
            return std::make_unique<Void>();
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs > rhs; }>(set);
        }
    }

    $ operator<=(Interface const &set) const override {
        if constexpr (isBool) {
            return std::make_unique<Void>();
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs <= rhs; }>(set);
        }
    }

    $ operator>=(Interface const &set) const override {
        if constexpr (isBool) {
            return std::make_unique<Void>();
        } else {
            return _binOp<[](auto lhs, auto rhs) { return lhs >= rhs; }>(set);
        }
    }

    $ operator||(Interface const &set) const override {
        if constexpr (isBool) {
            return _binOp<[](auto lhs, auto rhs) { return lhs && rhs; }>(set);
        } else {
            return std::make_unique<Void>();
        }
    }

    $ operator&&(Interface const &set) const override {
        if constexpr (isBool) {
            return _binOp<[](auto lhs, auto rhs) { return lhs || rhs; }>(set);
        } else {
            return std::make_unique<Void>();
        }
    }

    $ operator==(Interface const &set) const override {
        return _binOp<[](auto lhs, auto rhs) { return lhs == rhs; }>(set);
    }

    $ operator!=(Interface const &set) const override {
        return _binOp<[](auto lhs, auto rhs) { return lhs != rhs; }>(set);
    }

    $ contains(Interface const &set) const override { return thisset() == set.superset(); }

    bool ok() const override { return true; }

    $ extract(std::string const & /*name*/) const override { return std::make_unique<Failure>(); }

    $ clone() const override { return std::make_unique<Base<T>>(*this); }

    std::string show() const override { return std::format("{}", _val); }

    T value() const { return _val; }

private:
    template <auto BinOp> $ _binOp(Interface const &set) const {
        if (&superset() != &set.superset()) return std::make_unique<Void>();
        auto res = BinOp(_val, set.thisset().cast<Base<T>>()._val);
        return std::make_unique<Base<decltype(res)>>(res);
    }

private:
    T _val;
};

using Bool = Base<bool>;
using Int = Base<int>;

struct Every : Interface {
    constexpr static Identity const id = {};
    Interface const &thisset() const override { return id; } // ref of identity
    Interface const &superset() const override { return Universe::id; }

    $ operator+(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator-(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator*(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator/(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator!() const override { return std::make_unique<Void>(); }
    $ operator-() const override { return std::make_unique<Void>(); }

    $ operator<(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator>(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator<=(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator>=(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator||(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator&&(Interface const & /*set*/) const override { return std::make_unique<Void>(); }

    $ operator==(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ operator!=(Interface const & /*set*/) const override { return std::make_unique<Void>(); }
    $ contains(const Interface & /*set*/) const override { return std::make_unique<Bool>(true); }
    bool ok() const override { return true; }
    $ extract(std::string const & /*name*/) const override { return std::make_unique<Failure>(); }

    $ clone() const override { return std::make_unique<Every>(); }
    std::string show() const override { return "everyset"; }
};

class Ref : public Interface {
public:
    Ref(Interface const &set) : _set(set.thisset()) {}
    Interface const &thisset() const override { return _set; }
    Interface const &superset() const override { return _set.superset(); }

    $ operator+(Interface const &set) const override { return _set + set; }
    $ operator-(Interface const &set) const override { return _set - set; }
    $ operator*(Interface const &set) const override { return _set * set; }
    $ operator/(Interface const &set) const override { return _set / set; }
    $ operator!() const override { return !_set; }
    $ operator-() const override { return -_set; }

    $ operator<(Interface const &set) const override { return _set < set; }
    $ operator>(Interface const &set) const override { return _set > set; }
    $ operator<=(Interface const &set) const override { return _set <= set; }
    $ operator>=(Interface const &set) const override { return _set >= set; }
    $ operator||(Interface const &set) const override { return _set || set; }
    $ operator&&(Interface const &set) const override { return _set && set; }

    $ operator==(Interface const &set) const override { return _set == set; }
    $ operator!=(Interface const &set) const override { return _set != set; }
    $ contains(Interface const &set) const override { return _set.contains(set); }
    bool ok() const override { return true; }
    $ extract(std::string const &name) const override { return _set.extract(name); }

    $ clone() const override { return std::make_unique<Ref>(_set); }
    std::string show() const override { return _set.show(); }

private:
    Interface const &_set;
};

class Sets : public Interface {
public:
    Interface const &thisset() const override { return *this; }
    Interface const &superset() const override { return Universe::id; }

    $ operator+(Interface const & /*set*/) const override { return nullptr; }
    $ operator-(Interface const & /*set*/) const override { return nullptr; }
    $ operator*(Interface const & /*set*/) const override { return nullptr; }
    $ operator/(Interface const & /*set*/) const override { return nullptr; }
    $ operator!() const override { return nullptr; }
    $ operator-() const override { return nullptr; }

    $ operator<(Interface const & /*set*/) const override { return nullptr; }
    $ operator>(Interface const & /*set*/) const override { return nullptr; }
    $ operator<=(Interface const & /*set*/) const override { return nullptr; }
    $ operator>=(Interface const & /*set*/) const override { return nullptr; }
    $ operator||(Interface const & /*set*/) const override { return nullptr; }
    $ operator&&(Interface const & /*set*/) const override { return nullptr; }

    $ operator==(Interface const & /*set*/) const override { return nullptr; }
    $ operator!=(Interface const & /*set*/) const override { return nullptr; }
    $ contains(Interface const & /*set*/) const override { return nullptr; }
    bool ok() const override {
        return std::ranges::all_of(_data, [](auto const &pair) { return pair.second->ok(); });
    }

    $ extract(std::string const &name) const override;
    void add(std::string_view const &name, $ &&set) { _data.insert({name, std::move(set)}); }

    $ clone() const override { return std::make_unique<Ref>(thisset()); }
    std::string show() const override { return "sets"; }

private:
    std::map<std::string_view, $> _data;
};

// impl

template <typename S = Failure, typename... Args> static Set create(Args &&...args) {
    return Set{std::make_unique<S>(std::forward<Args>(args)...)};
}

// Set
template <typename T> requires derived<T> std::unique_ptr<T> Set::move() {
    auto out = std::make_unique<T>(std::move(_set.release()->cast<T>()));
    _set = std::make_unique<Failure>();
    return out;
}

inline $ Set::move() {
    auto out = std::move(_set);
    _set = std::make_unique<Failure>();
    return out;
}

// Identity
inline Interface const &Identity::superset() const {
    return Universe::id;
}

inline $ Identity::operator&&(Interface const & /*set*/) const {
    return std::make_unique<Void>();
}

inline $ Identity::operator||(Interface const & /*set*/) const {
    return std::make_unique<Void>();
}

inline $ Identity::operator+(Interface const & /*set*/) const {
    return std::make_unique<Void>();
}

inline $ Identity::operator-(Interface const & /*set*/) const {
    return std::make_unique<Void>();
}

inline $ Identity::operator*(Interface const & /*set*/) const {
    return std::make_unique<Void>();
}

inline $ Identity::operator/(Interface const & /*set*/) const {
    return std::make_unique<Void>();
}

inline $ Identity::operator<(Interface const & /*set*/) const {
    return std::make_unique<Void>();
}

inline $ Identity::operator>(Interface const & /*set*/) const {
    return std::make_unique<Void>();
}

inline $ Identity::operator<=(Interface const & /*set*/) const {
    return std::make_unique<Void>();
}

inline $ Identity::operator>=(Interface const & /*set*/) const {
    return std::make_unique<Void>();
}

inline $ Identity::operator!() const {
    return std::make_unique<Void>();
}

inline $ Identity::operator-() const {
    return std::make_unique<Void>();
}

inline $ Identity::operator!=(Interface const &set) const {
    return std::make_unique<Bool>(this != &set.thisset());
}

inline $ Identity::operator==(Interface const &set) const {
    return std::make_unique<Bool>(this == &set.thisset());
}

inline $ Identity::extract(std::string const & /*name*/) const {
    return std::make_unique<Failure>();
}

inline $ Identity::clone() const {
    return std::make_unique<Ref>(*this);
}

// Void
inline Interface const &Void::superset() const {
    return Every::id;
}

inline $ Void::operator!() const {
    return std::make_unique<Universe>();
}

// Universe
inline $ Universe::contains(Interface const & /*set*/) const {
    return std::make_unique<Bool>(true);
}

// Sets
inline $ Sets::extract(std::string const &name) const {
    auto findset = _data.find(name);
    if (findset != _data.end()) {
        return findset->second->clone();
    }
    return std::make_unique<Failure>();
}

} // namespace set
