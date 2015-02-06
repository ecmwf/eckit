from sys import argv

camelcase = lambda s: ''.join([w.capitalize() for w in s.split('_')])
f = lambda s: 'xpr::' + s

xpr_operator_tpl = """
BOOST_AUTO_TEST_CASE( test_{op_c}_{a_c}_{b_c} )
{{
    Xpr A = Xpr({a}) {sym} Xpr({b});
    Xpr B = {a} {sym} Xpr({b});
    Xpr C = Xpr({a}) {sym} {b};
    Xpr D = {a} {sym} {b};

    BOOST_CHECK_EQUAL( extract<{res_c}>(A.eval()) , {res} );
    BOOST_CHECK_EQUAL( extract<{res_c}>(B.eval()) , {res} );
    BOOST_CHECK_EQUAL( extract<{res_c}>(C.eval()) , {res} );
    BOOST_CHECK_EQUAL( extract<{res_c}>(D.eval()) , {res} );

    BOOST_CHECK_EQUAL( A.str() , "{op_c}({a_c}({ad}), {b_c}({bd}))" );
    BOOST_CHECK_EQUAL( B.str() , "{op_c}({a_c}({ad}), {b_c}({bd}))" );
    BOOST_CHECK_EQUAL( C.str() , "{op_c}({a_c}({ad}), {b_c}({bd}))" );
    BOOST_CHECK_EQUAL( D.str() , "{res_c}({res})" );

    BOOST_CHECK_EQUAL( A.code() , "{op_f}({a_f}({ad}), {b_f}({bd}))" );
    BOOST_CHECK_EQUAL( B.code() , "{op_f}({a_f}({ad}), {b_f}({bd}))" );
    BOOST_CHECK_EQUAL( C.code() , "{op_f}({a_f}({ad}), {b_f}({bd}))" );
    BOOST_CHECK_EQUAL( D.code() , "{res_f}({res})" );

    BOOST_CHECK_EQUAL( A.json() , "{{\\"{op_f}\\":[{ad},{bd}]}}" );
    BOOST_CHECK_EQUAL( B.json() , "{{\\"{op_f}\\":[{ad},{bd}]}}" );
    BOOST_CHECK_EQUAL( C.json() , "{{\\"{op_f}\\":[{ad},{bd}]}}" );
    BOOST_CHECK_EQUAL( D.json() , "{res}" );
}}
"""

def gen_operator_tests(testfile):
    tests = []
    arithmetic_ops = [('add', '+'), ('sub', '-'), ('div', '/'), ('prod', '*')]
    arithmetic_args = [(2, 'integer', 4., 'real', 'real'),
                       (2., 'real', 4, 'integer', 'real'),
                       (2., 'real', 4., 'real', 'real'),
                       (2, 'integer', 4, 'integer', 'integer')]
    for op, sym in arithmetic_ops:
        for a, a_t, b, b_t, res_t in arithmetic_args:
            res = eval("%s %s %s" % (a, sym, b))
            # Real expressions are represented as int if they're not decimal
            if res == int(res):
                res = int(res)
            tests.append(xpr_operator_tpl.format(a=a,
                                                 ad=int(a),
                                                 a_c=camelcase(a_t),
                                                 a_f=f(a_t),
                                                 b=b,
                                                 bd=int(b),
                                                 b_c=camelcase(b_t),
                                                 b_f=f(b_t),
                                                 op_c=camelcase(op),
                                                 op_f=f(op),
                                                 res=res,
                                                 res_c=camelcase(res_t),
                                                 res_f=f(res_t),
                                                 sym=sym))
    return testfile % {'test_cases': '\n\n'.join(tests)}

if __name__ == '__main__':
    with open(argv[1]) as fin, open(argv[2], 'w') as fout:
        fout.write(gen_operator_tests(fin.read()))
