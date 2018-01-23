# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('quic', ['core'])
    module.source = [
        'model/quic.cc',
        'helper/quic-helper.cc',
        ]

    module_test = bld.create_ns3_module_test_library('quic')
    module_test.source = [
        'test/quic-test-suite.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'quic'
    headers.source = [
        'model/quic.h',
        'helper/quic-helper.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # bld.ns3_python_bindings()

