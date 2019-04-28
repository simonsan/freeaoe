%%

aiscript:
    /* Empty */
    | rules { printf("got script\n"); }
;

rules:
    rule { printf("got single rule\n"); }
    | rule rules { printf("got multiple rules\n"); }

rule:
    OpenParen RuleStart conditions ConditionActionSeparator actions CloseParen { printf("got rule\n====\n\n"); }

conditions:
    condition {  printf("got single condition\n"); }
    | condition conditions {  printf("got multiple conditions\n"); }

condition:
    OpenParen Not condition CloseParen {  printf("got negated condition\n"); }
    | OpenParen Or conditions CloseParen {  printf("got multiple or conditions\n"); }
    | OpenParen SymbolName CloseParen {  printf("got condition with symbol '%s'\n", $2); }
    | OpenParen SymbolName Number CloseParen {  printf("got condition with symbol '%s' and number %d\n", $2, $3); }
    | OpenParen SymbolName Number Number CloseParen {  printf("got condition with symbol '%s' and numbers %d %d\n", $2, $3, $4); }
    | OpenParen SymbolName SymbolName CloseParen {  printf("got condition with two symbols '%s' %s\n", $2, $3); }
    | OpenParen SymbolName SymbolName SymbolName CloseParen {  printf("got condition with three symbols %s %s %s\n", $2, $3, $4); }
    | OpenParen SymbolName comparison Number CloseParen {  printf("got condition with comparison %s %d\n", $2, $4); }
    | OpenParen SymbolName comparison SymbolName CloseParen {  printf("got condition with comparison %s %s\n", $2, $4); }
    | OpenParen SymbolName SymbolName comparison SymbolName CloseParen {  printf("got condition with comparison %s %s %s\n", $2, $3, $5); }
    | OpenParen SymbolName SymbolName comparison Number CloseParen {  printf("got condition with comparison %s %s %d\n", $2, $3, $5); }
    | OpenParen SymbolName SymbolName SymbolName comparison Number CloseParen {  printf("got condition with comparison %s %s %s %d\n", $2, $3, $4, $6); }


comparison:
    RelOpLessThan {  printf("got lessthan\n"); }
    | RelOpLessOrEqual {  printf("got lessorequal\n"); }
    | RelOpGreaterThan {  printf("got greaterthan\n"); }
    | RelOpGreaterOrEqual {  printf("got greaterorequal\n"); }
    | RelOpEqual {  printf("got equals\n"); }

actions:
    action {  printf("got single action\n"); }
    | action  actions {  printf("got multiple actions\n"); }

action:
    OpenParen SymbolName CloseParen { printf("got action %s without arguments\n", $2); }
    | OpenParen SymbolName String CloseParen {  printf("got action %s with string %s\n", $2, $3); }
    | OpenParen FnSetStrategicNumber strategicnumber Number CloseParen {  printf("got action  with symbol and number %d\n",  $4); }
    | OpenParen SymbolName SymbolName Number CloseParen {  printf("got action %s with symbol %s and number %d\n", $2, $3, $4); }
    | OpenParen SymbolName SymbolName CloseParen {  printf("got action %s with symbol %s\n", $2, $3); }
    | OpenParen SymbolName Number CloseParen {  printf("got action %s with number %d\n", $2, $3); }
    | OpenParen SymbolName Number Number CloseParen {  printf("got action %s with numbers %d %d\n", $2, $3, $4); }
