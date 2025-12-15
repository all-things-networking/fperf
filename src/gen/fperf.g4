grammar fperf;

con : interval ':' lhs comp_op rhs ;
lhs : m '(' q ', t)' | mm '(' q ', t)' | 'SUM_[q in' set ']' m '(q ,t)';
m : 'cenq' | 'aipg';
mm: 'ecmp' | 'dst';
q : INT ;
rhs : INT | 't' | INT 't' ;
interval     : '[' INT ',' INT ']';
set          : '{' (INT (',' INT)*)? ','? '}';
comp_op      : '>=' | '<=' | '>' | '<' | '==' | '!=';

ID           : [a-zA-Z_][a-zA-Z0-9_]*;
INT          : [0-9]+;
WS           : [ \t\r\n]+ -> skip;
