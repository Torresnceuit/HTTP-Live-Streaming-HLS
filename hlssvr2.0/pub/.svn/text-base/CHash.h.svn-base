#ifndef _CHASH_H
#define _CHASH_H

template <typename TNodeType, unsigned int UICHASHLEN>
class CHash
{
public:
    CHash()
    {
        memset(this, 0, sizeof(*this));
    }

    TNodeType &get(unsigned int uihashKey)
    {
        return m_pHash[uihashKey%UICHASHLEN];
    }

    const TNodeType &get_const(unsigned int uihashKey) const 
    {
        return m_pHash[uihashKey%UICHASHLEN];
    };

    int del(unsigned int uihashKey)
    {
        memset(&m_pHash[uihashKey%UICHASHLEN], 0, sizeof(TNodeType));
        return 0;
    }

    TNodeType &ins(unsigned int uihashKey, const TNodeType& oNode)
    {
        unsigned int uiIndex = uihashKey%UICHASHLEN;
        m_pHash[uiIndex] = oNode;
        return m_pHash[uiIndex];
    }

    unsigned int hashlen(void) const 
    {
        return UICHASHLEN;
    }

    int get_buffer(const TNodeType **ppBuffer, unsigned int *puiBufferSize) const
    {
        *ppBuffer = (const TNodeType *)&m_pHash;
        *puiBufferSize = UICHASHLEN;
        return 0;
    }

private:
    TNodeType m_pHash[UICHASHLEN];
};

#endif

