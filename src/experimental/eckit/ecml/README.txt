ECML
-----------------------------------------

ECML is an embedded and extensible scripting language with a syntax 
compatible with syntax of the ECMWF MARS language. 

A script contains a sequence of requests, each beginning with a verb
and followed by a list of attributes and their values.

A value of an attribute is always a list, containing strings and
other requests, separated with a slash ('/').

Requests appearing on the list of values must be in parantheses.


Evaluation rules of ECML
-----------------------------------------

Script is parsed and requests are executed in sequence.

Execution is performed in an execution context.
The most important part of execution context is environment,
which is a stack of dictionaries mapping names to objects like data
or functions (verbs).

Evaluating a single request depends on whether its verb is a regular
verb, or a special form.

1) If this is a regular verb, for example archive, retrieve (implemented
in C++ as a RequestHandler or defined with a 'function' request) then:

 - values of attributes are first evaluated.
   Lists of values produced by evaluated requests nested on the values 
   lists are concatenated together.

 - a new dictionary is put on the environment; the dictionary is populated
   with bindings of atributes of the request and evaluated lists of values. 

 - a RequestHandler::handle(ExecutionContext&) implementation mapped to 
   the verb is executed. The C++ code may use context.environment() to lookup
   values it needs to do something and return a value.
   After executing body of the function the environment created
   in previous step is popped from the environment.

2) If this is a special form, for example 'let' then the interpreter
passes the parsed, unevaluated request and execution context to
a registered SpecialFormHandler(Request, ExecutionContext&).

Currently there are two special forms: 'let' and 'function'.

'Let' (implemented in LetHandler) pushes a new dictionary containing
bindings of attributes to evaluated lists of values. The dictionary will
stay on top of the environment stack till the last request of 
the currently executed sequence of requests is executed (last request 
in a script file for the top level requests, or a closing paren for requests 
embedded on a values list).

Let is simply a way to introduce new variables in the script.
The variables can be retrieved from evironment using a regular verb
'value', for example (value,of=x) returns value associated with variable x.

'Function' is a special form used to define new verbs.
It accepts one or two attributes.
The first, optional, attribute 'of' contains a list of parameters
of the new function.
The second atribute is a name of the new verb. The value of the attribute
is the code of the new function.
The scope of a function defined with a 'function' request has the same scope
as variables introduced with 'let'.

