/********************************************************************************************************
 * PROGRAM	  : 
 * DATE - TIME  : lundi 10 avril 2006 - 22:28
 * AUTHOR	   : IComplete Team and Anacr0x ( fred.julian at gmail.com )
 * FILENAME	 : 
 * LICENSE	  : GPL
 * COMMENTARY   : Modified file (with qt) of the icomplete project
 ********************************************************************************************************/
#ifndef __TREE_H__
#define __TREE_H__

#include <QStringList>
#include <QFile>

#include "../InitCompletion.h"
#include "parse.h"

typedef enum
{
    NOT_FOUND = 0,
    DIRECT_HIT,
    SUBCLASS
} Match;

/* this struct holds the inheritance information */
class Tree
{
public:
	static InitCompletion *parent;

	QString name;			// the name of the class
	QList<Tree*> inherits;		// next elements in the tree

	/*
	* Build an inheritance tree for classes
	* Looks like:
	*
	*             --- subclass1
	* rootclass <
	*             --- subclass2 -- subclass2_1 --> subclass3
	*                            \ subclass2_2
	*
	* Caller must free the tree with
	* freeTree() after calling this function
	*/
	static Tree *buildInheritanceTree(const QString &className);

	/* free()'s all memory allocated of a Tree* */
	void freeTree();

	/*
	    * This function searches a given Tree for a className name
	    * and returns if the className is the root element of the tree
	    * (=DIRECT_HIT), or a subclass
	 */
	Match findInTree(const QString &className);

	/* when we find a tag, the following conditions must be met, to show the tag:
	    * - method must be part of a className (key=className)
	    * - access permissions (private, protected, public) must match.
	    * - FIXME: method must not be pure virtual
	    * - no static method
	 */
	bool isMemberOfScope(const tagEntry * entry, const Scope * scope);

	/*
	     * parses a tags file to show all className members
	     * relative to the scope of another className
	 */
	static QList < Tag > findEntries(const Expression * exp, const Scope * scope);

private:
	void addTreeChild(const QString &className);
};

#endif				/* __TREE_H__ */
