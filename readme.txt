The compilation is done with the make command, and the program is executed by typing ./mysh .

The following .h files are included:

functions.h: Contains the main functions that implement the interface and features of the shell.
helperfunc.h: Contains helper functions for creating and printing character arrays used in the implementation.
signalhandler.h: Contains functions for handling signals.
builtIn.h: Contains implementations for the shell's built-in functionalities.
The delimiter for separating words from user input is a space, so redirection, piping, background execution, and the Greek question mark characters must be separated by a space from commands or files in the command. For example, ls -l > ls.txt instead of ls -l>ls.txt, cat < ls.txt | sort instead of cat < ls.txt|sort, and ls -l ; sort ls.txt.

Only the background execution character can be used without a space when followed by the Greek question mark, i.e., count1 &; count2 is acceptable instead of count1&;count2. When the background process finishes, the user presses Enter, as in bash, and the mysh shell returns a message indicating the process's completion.

For the myHistory command, the shell keeps a session history of commands entered in two ways: by using the readline/history.h library (allowing users to navigate previous commands with the up-down arrows) and via the built-in myHistory command. Using myHistory prints the last X (HISTORY_NUMS set in builtIn.h) commands, with the numbering increasing to show the most recent command. The numbering resets, replacing old commands one by one; for instance, the 21st command will go to position 1, replacing the old 1st command, and so on (assuming X = 20). myHistory is not stored in its list but is stored in the list accessible with the keyboard arrows. Using myHistory 4 (without quotes) executes the 4th stored command, displaying it before execution. The history list does not store built-in commands like myHistory, create/destroyalias, myAliases, cd, exit.

For implementing pipelines, multiple command pipelines are supported, with the option to read from a file in the first command of the pipeline and to write to a file in the last command.

In commands like grep "stef", the word stef should not be enclosed in quotes.

Programs can be executed with either the relative or absolute path of the executable.

Regarding aliases, the shell keeps aliases created in the current session, with a maximum of X (ALIASES_NUM defined in builtIn.h). The commands for creating and destroying aliases are createalias and destroyalias, respectively. The myAliases command also displays the aliases created by the user (similar to myHistory). To create an alias for a command without arguments (e.g., cat, myout, a.out), the user enters createalias myal ls. If the alias should include command arguments, the command and arguments must be enclosed in quotes, e.g., createalias myal "ls -las" or createalias myal "ls -l -s". If an alias to be destroyed does not exist, an appropriate message is displayed. If a command that is neither a program nor an alias is used, the shell displays a relevant message. Aliases cannot be created for built-in commands.
==============================================================================================================================================================================================================================

Το compilation γίνεται με την εντολή make και η εκτέλεση του προγράμματος γράφοντας ./mysh .

Υπάρχουν τα εξής .h αρχεία:
functions.h: που περιέχει τις κεντρικές συναρτήσεις που υλοποιούν τη διεπαφή και τα χαρακτηριστικά του κελύφους.
helperfunc.h: περιέχει βοηθητικές συναρτήσεις για τη δημιουργία και εκτύπωση πινάκων char που χρησιμοποιούνται στην υλοποίηση.
signalhandler.h: περιέχει συναρτήσεις που διαχειρίζονται σήματα.
builtIn.h: περιέχει υλοποιήσεις για τις built in λειτουργίες του κελύφους.

Ως delimiter για τον χωρισμό των λέξεων απο το input του χρήστη είναι το κενό (space), επομένως οι χαρακτήρες ανακατεύθυνσης, σωλήνωσης, background και ελληνικό ερωτηματικό πρέπει 
να χωρίζονται με κενό απο τις εντολές ή αρχεία που υπάρχουν στην εντολή, πχ "ls -l > ls.txt" όχι "ls -l>ls.txt", "cat < ls.txt | sort" όχι "cat < ls.txt| sort" και "ls -l ; sort ls.txt". 

Μόνο ο χαρακτήρας για εκτέλεση εντολής στο background μπορεί να μην έχει κενό με το ελληνικό ερωτηματικό, δηλαδή "count1 &; count2" είναι αποδεκτή εντολή αντί της "count1&;count2".
Όταν η background διαδικασία τελειώνει, ο χρήστης πατάει enter όπως και στο bash και το κέλυφος mysh επιστρέφει μήνυμα τελειώματος της διαδικασίας.

Για την εντολή myHistory. To κέλυφος κρατάει το ιστορικό του session για τις εντολές που δόθηκαν με δύο τρόπους, με τη χρήση της readline/history.h βιβλιοθήκης(o χρήστης μπορεί να 
πλοηγηθεί στις προηγούμενες εντολές με τα κάτω-πάνω βελάκια) αλλά και μέσω της builtIn εντολής myHistory. Η χρήση της εντολής myHistory εκτυπώνει τις Χ(HISTORY_NUMS ορισμένο σε builtIn.h)
προηγούμενες εντολές και όσο μεγαλώνει η αρίθμηση βρίσκεται η πιο πρόσφατη εντολή. Η αρίθμηση ανακυκλώνεται απο την αρχή και αντικαθίστανται οι παλίες εντολές μία μία,
δηλαδή η 21η εντολή θα πάει στη θέση 1 αντικαθιστώντας την παλιά 1η, η 22η την 2η κ.ο.κ.(με την υπόθεση ότι Χ = 20). Η myHistory δεν αποθηκεύεται στη λίστα που διατηρεί η ίδια, όμως αποθηκεύεται στη λίστα
που γίνεται προσπέλαση με τα βελάκια του πληκτρολογίου. Με τη χρήση "myHistory 4" (χωρίς τα "") το κέλυφος εκτελεί την 4η αποθηκευμένη εντολή αναγράφοντάς τη πρωτού την εκτελέσει.
Η λίστα του ιστορικού δεν αποθηκεύει builtIn εντολές, δηλαδή τις myHistory, create/destroyalias, myAliases, cd, exit.

Για την υλοποίηση των σωληνώσεων. Υποστηρίζεται πολλαπλή σωλήνωση εντολών, όπως και δυνατότητα διαβάσματος απο αρχείο στην πρώτη εντολή της σωλήνωσης αλλά και δυνατότητα εγγραφής 
σε αρχείο στην τελευταία εντολή.

Σε εντολές όπως η "grep "stef"" δεν πρέπει να περικλείεται η λέξη "stef" σε "αυτάκια".

Μπορεί να γίνει η εκτέλεση προγραμμάτων είτε με τη χρήση relative ή absolute path του εκτελέσιμου.

Για τα alises. Το κέλυφος διατηρεί τα aliases που δημιουργήθηκαν στο τωρινό session, με μέγιστο αριθμό Χ(ALIASES_NUM ορισμένο σε builtIn.h). Οι εντολές δημιουργίας και καταστροφής alias είναι createalias 
και destroyalias αντίστοιχα. Δίνεται επίσης και η εντολή myAliases που τυπώνει τα aliases που έχει δημιουργήσει ο χρήστης(παρόμοια όπως και η myHistory). Για τη δημιουργία των aliases, αν ο χρήστης θέλει 
να δημιουργήσει alias για μία εντολή χωρίς ορίσματα(πχ cat, myout, a.out κ.α) τότε δίνει την εντολή createalias myal ls. Αν θέλει το alias να περιέχει και τυχόν arguments της εντολής, πρέπει η εντολή
με τα argument να βρίσκονται μέσα σε αυτάκια, δηλαδή createalias myal "ls -las" ή createalias myal "ls -l -s". Σε περίπτωση καταστροφής alias που δεν υπάρχει, εκτυπώνεται σχετικό μήνυμα.
Σε περίπτωση χρήση εντολής που δεν αποτελεί πρόγραμμα ή alias, το κέλυφος βγάζει σχετικό μήνυμα. Επίσης, δεν μπορούν να δημιουργθηούν aliases για τις builtin εντολές.
