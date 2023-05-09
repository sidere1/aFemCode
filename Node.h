#ifndef DEF_NODE
#define DEF_NODE


class Node
{
public:
    Node();
    Node(int index, float x, float y, float z);
    Node(int index, float x, float y, float z, int groupId);
    ~Node();
    int getIndex();
    bool getDisplay() const;
    float getX() const;
    float getY() const;
    float getZ() const;

private:
    int m_id;
    int m_groupId;
    float m_coordinates[3];

};

#endif

/*

*/
