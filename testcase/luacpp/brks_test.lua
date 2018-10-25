
package.cpath = package.cpath .. ";/usr/local/lib/?.so;./brks_thrift/?.so"

require("libbrksclient")


local opt = {
    -- host      = '47.106.79.26',
    host      = '127.0.0.1',
    port      = '9090',
    -- protocol  = TCompactProtocol,
    -- transport = TFramedTransport
}

function testBasicClient()
    local ret = brks_client.brk_init(opt.host, opt.port)
    -- assertEqual(ret, -1, 'Failed call brk_init')
    print("[lua] brk_init ret: " .. ret)
    
    local r = brks_client.brk_get_mobile_code("15821725973")
    print(r.resultCode)
    print(r.resultMsg)
    assertEqual(r.resultCode, 200, 'Failed call brk_get_mobile_code')
end

testBasicClient()
-- teardown()
