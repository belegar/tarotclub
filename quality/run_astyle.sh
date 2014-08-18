# Run ASyle on the entire TarotClub source code

astyle --options=astyle/astyle.cfg --exclude=duktape.c --exclude=duktape.h --exclude=sqlite3.c --exclude=sqlite3.h --exclude=sqlite3ext.h --exclude=../src/desktop/forms -r "../src/*"

