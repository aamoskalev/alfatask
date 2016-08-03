#include <QCoreApplication>
#include <QFile>
#include <QVector>
#include <QTextCodec>
#include <QStringList>
#include <QQueue>

/*!
 \brief
 Class Node. Used for create tree of words

 \author
 Andrey Moskalev
 \version
 1.0
 \date
 July 2016

 The Node class is type of tree nodes, which presents all available chains of words.
 Every node accociated with one word from vocabulary.
 Specification to create child is difference of his word in one symbol with word of parent node.
*/

class Node {
public:
    /*!
    Constructor Creates Node for word from vocabulary
    \param[in] indexInVoc Index in vocabulary
    \param[in] parent     Pointer to parent node
    */
    Node (int indexInVoc, Node* parent) :
        _indexInVoc (indexInVoc),
        _parent     (parent) {

    }

    /*!
    Destructor Deletes Node and all his childs
    */
    ~Node () {
        while (!_childs.empty()) {
            delete _childs.front ();
            _childs.pop_front ();
        }
    }

    int            _indexInVoc; ///< Index of accociated word in vocabulary
    QVector<Node*> _childs;     ///< Array of childs
    Node*          _parent;     ///< Parent of all available chains
};

/*!
\brief
Check is node of word can be linked (child) with previous one
\param[in] word Word of node for which checking perform
\param[in] val  Available value from vocabulary
\return         Word differed from val in one symbol
*/
bool isChild (QString& word, QString& val) {
    int size = word.size ();
    bool dif = false;
    for (int i = 0; i < size; ++i) {
        if (word.at (i) != val.at (i)) {
            if (dif)
                return false;
            dif = true;
        }
    }
    return dif ? true : false;
}

/*!
\brief
Processing of tree in width to create all available chains of words and return first succesed
\param[in,out] iterationChilds Container to store nodes for futher processing
\param[in]     vocabulary      Container which stores all words from vocabulary
\return                        Pointer to node with final result word
*/
//
Node* createChilds (QQueue<Node*>& iterationChilds, QStringList& vocabulary) {
    Node* node = iterationChilds.dequeue ();
    QString word = vocabulary.at (node->_indexInVoc);
    int size = vocabulary.size ();
    for (int i = 1; i < size; ++i) {
        if (node->_parent && i == node->_parent->_indexInVoc)
            continue;
        if (isChild (word, vocabulary [i])) {
            Node* newNode = new Node (i, node);
            node->_childs.append (newNode);
            iterationChilds.enqueue (newNode);
            if (i == (size - 1))
                return newNode;
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    printf ("Andrey Moskalev issue\n");
    // Read start, end words and vocabulary
    QString start;
    QString end;
    QStringList vocabulary;
    // Codec of files
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    QFile words ("words.txt");
    if (words.open (QIODevice::ReadOnly)) {
        QString arr = codec->toUnicode(words.readAll ());
        QString arr1 = arr.simplified (); // remove enter, more than 1 space and tab
        QStringList l = arr1.split(' '); // move text array to list with space splitter
        start = l.at (0);
        end = l.at (1);
        words.close ();
    } else {
        return 0;
    }
    int stSize = start.size();
    QFile vocab ("vocab.txt");
    if (vocab.open (QIODevice::ReadOnly)) {
        QString arr = codec->toUnicode(vocab.readAll ());
        QString arr1 = arr.simplified (); // remove enter, more than 1 space and tab
        vocabulary = arr1.split(' '); // move text array to list with space splitter
        vocab.close ();
    }
    // remove from vocabulary words of different size than targeted
    for (int i = 0; i < vocabulary.size (); ++i) {
        if (vocabulary [i].size()>stSize || vocabulary [i] == start || vocabulary [i] == end) {
            vocabulary.removeAt(i);
            --i;
        }
    }
    vocabulary.prepend (start);
    vocabulary.append (end);


    // Create root node
    Node* node = new Node (0, NULL);
    QQueue<Node*> iterationChilds;
    iterationChilds.enqueue (node);
    Node* finished = NULL;
    // Create and processing tree
    while (!finished) {
        finished = createChilds (iterationChilds, vocabulary);
    };

    // Create first succesed final sequence of words
    QStringList result;
    while (finished) {
        result.prepend (vocabulary.at (finished->_indexInVoc));
        finished = finished->_parent;
    }
    // delete tree
    delete node;


    // Print result
    foreach (QString val, result) {
        printf ("%s\n", codec->fromUnicode(val).data ());
    }

    return a.exec();
}
