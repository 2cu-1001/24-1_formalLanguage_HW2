Project file of DFA2NFA converter
implemented with C++


example of Input and Output format :

	StateSet = { q000, q001, q002, q003, q004 }
	TerminalSet = { 0, 1 }
	DeltaFunctions = {
		(q000, 0) = {q001, q002}
		(q000, 1) = {q001, q003}
		(q001, 0) = {q001, q002}
		(q001, 1) = {q001, q003}
		(q002, 0) = {q004}
		(q003, 1) = {q004}
		(q004, 0) = {q004}
		(q004, 1) = {q004}
	}
	StartState = q000
	FinalStateSet = { q004 }
