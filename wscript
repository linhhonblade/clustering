# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('clustering', ['core', 'wave'])
    module.source = [
        'model/clustering-position.cc',
        'model/clustering-header.cc',
        'model/clustering-utils.cc',
        'model/clustering-client.cc',
        'helper/clustering-client-helper.cc'
        ]

    module_test = bld.create_ns3_module_test_library('clustering')
    module_test.source = [
        'test/clustering-test-suite.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'clustering'
    headers.source = [
        'model/clustering-position.h',
        'model/clustering-header.h',
        'model/clustering-utils.h',
        'model/clustering-client.h',
        'helper/clustering-client-helper.h'
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # bld.ns3_python_bindings()

