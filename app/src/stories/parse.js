
function c(e, t) {
    for (var n = -1, r = null == e ? 0 : e.length, i = 0, o = []; ++n < r;) {
        var a = e[n]; t(a, n, e) && (o[i++] = a)
    } return o
}

function p(e, t, n) {
    for (var r = -1, i = null == e ? 0 : e.length; ++r < i;)if (n(t, e[r])) return !0; return !1
}


export function a(e, t) {
    if (Array.isArray(t)) return "object" === c(t[0]) ? Object.freeze(t[0]) : t[0];
    if (Array.isArray(t)) return Object.freeze(t.map(function (e) {
        return a(null, e)
    }));

    if ("object" !== (void 0 === t ? "undefined" : c(t)) || null === t) return t;
    var n = void 0;
    if (Array.isArray(e)) {
        n = [];
        var r = !0, i = !1, o = void 0;
        try {
            for (var u, s = (t.$ || [[0, e.length]])[Symbol.iterator]();
                !(r = (u = s.next()).done); r = !0) {
                var l = u.value;
                if (Array.isArray(l)) for (var f = l[0]; f < l[0] + l[1]; f++)n.push(e[f]); else -1 === n[l] ? n.push(void 0) : n.push(e[l])
            }
        } catch (e) {
            // i = !0, 
            // o = e
        } finally {
            try {
                !r && s.return && s.return()
            } finally {
                if (i) throw o
            }
        }
        delete t.$;
        var d = !0, h = !1, v = void 0;
        try {
            for (var m, g = Object.keys(t)[Symbol.iterator](); !(d = (m = g.next()).done); d = !0) {
                var y = m.value; n[y] = a(n[y], t[y])
            }
        } catch (e) {
            // h = !0, 
            // v = e
        } finally {
            try {
                !d && g.return && g.return()
            } finally {
                if (h) throw v
            }
        }
    } else {
        n = "object" === (void 0 === e ? "undefined" : c(e)) && null !== e ? (0, p.assign)({}, e) : {}; var b = !0, _ = !1, T = void 0;
        try {
            for (var w, C = Object.keys(t)[Symbol.iterator](); !(b = (w = C.next()).done); b = !0) {
                var x = w.value, k = t[x]; Array.isArray(k) && 0 === k.length ? delete n[x] : n[x] = a(n[x], k)
            }
        } catch (e) {
            // _ = !0, 
            // T = e
        } finally {
            try {
                !b && C.return && C.return()
            } finally {
                if (_) throw T
            }
        }
    }
    return Object.freeze(n)
} 