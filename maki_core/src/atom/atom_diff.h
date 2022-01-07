#pragma once

#include <glm/glm.hpp>

#include "atom/atom.h"

namespace Maki {

// how to convert atom "across" frame-border
template<typename AtomType>
class AtomDiff {
public:
    AtomDiff(uint32_t id)
        : m_id(id), m_priority(s_next_priority++) {}

    virtual ~AtomDiff() = default;

    uint32_t get_id() const { return m_id; }
    uint32_t get_priority() const { return m_priority; }

    virtual void apply(AtomType& atom) const   = 0;
    virtual void reverse(AtomType& atom) const = 0;

private:
    const uint32_t m_id;
    const uint32_t m_priority;

private:
    using s_atom_type = AtomType;
    static thread_local uint32_t s_next_priority;
};

template<typename AtomType>
thread_local uint32_t AtomDiff<AtomType>::s_next_priority {0};

// used for sorting containers
template<typename AtomType>
class CompareAtomDiff {
public:
    bool operator()(const AtomDiff<AtomType>* a, const AtomDiff<AtomType>* b) const
    {
        if(a->get_id() < b->get_id())
            return true;
        if(a->get_id() > b->get_id())
            return false;
        // consult priority when id same
        if(a->get_priority() <= b->get_priority())
            return true;
        return false;
    }
};

template<typename AtomType>
class ToggleRenderDiff: public AtomDiff<AtomType> {
public:
    ToggleRenderDiff(uint32_t id)
        : AtomDiff<AtomType>(id) {}

    virtual void apply(AtomType& atom) const override
    {
        atom.render = !atom.render;
    }
    virtual void reverse(AtomType& atom) const override
    {
        atom.render = !atom.render;
    }
};

template<typename AtomType>
class ReplacementDiff: public AtomDiff<AtomType> {
public:
    ReplacementDiff(uint32_t id)
        : AtomDiff<AtomType>(id) {}

    virtual void apply(AtomType& atom) const override
    {
        atom += m_diff;
    }
    virtual void reverse(AtomType& atom) const override
    {
        atom -= m_diff;
    }

private:
    AtomType m_diff;
};

template<typename AtomType>
class TransformDiff: public AtomDiff<AtomType> {
public:
    TransformDiff(uint32_t id)
        : AtomDiff<AtomType>(id) {}

    TransformDiff(mat4 mat)
        : m_mat(mat)
    {
        m_inv_mat = glm::inverse(mat);
    }

    virtual void apply(AtomType& atom) const override
    {
        for(size_t i {0}; i != atom.ver_pos; i += 3) {
            // TODO: should use move operations instead
            // TODO: fix code duplication
            vec4 ver_pos {
                atom.ver_pos[i + 0],
                atom.ver_pos[i + 1],
                atom.ver_pos[i + 2],
                1.0f};
            ver_pos             = m_mat * vec4();
            atom.ver_pos[i + 0] = ver_pos.x;
            atom.ver_pos[i + 1] = ver_pos.y;
            atom.ver_pos[i + 2] = ver_pos.z;
        }
    }
    virtual void reverse(AtomType& atom) const override
    {
        for(size_t i {0}; i != atom.ver_pos; i += 3) {
            // TODO: should use move operations instead
            vec4 ver_pos {
                atom.ver_pos[i + 0],
                atom.ver_pos[i + 1],
                atom.ver_pos[i + 2],
                1.0f};
            ver_pos              = m_inv_mat * vec4();
            atom.ver_post[i + 0] = ver_pos.x;
            atom.ver_post[i + 1] = ver_pos.y;
            atom.ver_post[i + 2] = ver_pos.z;
        }
    }

private:
    mat4 m_mat;
    mat4 m_inv_mat;
};

} // namespace Maki
