test("basic numeric and", () => {
    expect(0 & 0).toBe(0);
    expect(0 & 1).toBe(0);
    expect(0 & 2).toBe(0);
    expect(0 & 3).toBe(0);
    expect(0 & 4).toBe(0);
    expect(0 & 5).toBe(0);

    expect(1 & 0).toBe(0);
    expect(1 & 1).toBe(1);
    expect(1 & 2).toBe(0);
    expect(1 & 3).toBe(1);
    expect(1 & 4).toBe(0);
    expect(1 & 5).toBe(1);

    expect(2 & 0).toBe(0);
    expect(2 & 1).toBe(0);
    expect(2 & 2).toBe(2);
    expect(2 & 3).toBe(2);
    expect(2 & 4).toBe(0);
    expect(2 & 5).toBe(0);

    expect(3 & 0).toBe(0);
    expect(3 & 1).toBe(1);
    expect(3 & 2).toBe(2);
    expect(3 & 3).toBe(3);
    expect(3 & 4).toBe(0);
    expect(3 & 5).toBe(1);

    expect(4 & 0).toBe(0);
    expect(4 & 1).toBe(0);
    expect(4 & 2).toBe(0);
    expect(4 & 3).toBe(0);
    expect(4 & 4).toBe(4);
    expect(4 & 5).toBe(4);

    expect(5 & 0).toBe(0);
    expect(5 & 1).toBe(1);
    expect(5 & 2).toBe(0);
    expect(5 & 3).toBe(1);
    expect(5 & 4).toBe(4);
    expect(5 & 5).toBe(5);
});

test("and with non-numeric values", () => {
    let x = 3;
    let y = 7;

    expect("42" & 6).toBe(2);
    expect(x & y).toBe(3);
    expect(x & [[[[13]]]]).toBe(1);
    expect(undefined & y).toBe(0);
    expect("a" & "b").toBe(0);
    expect(null & null).toBe(0);
    expect(undefined & undefined).toBe(0);
    expect(NaN & NaN).toBe(0);
    expect(NaN & 6).toBe(0);
    expect(Infinity & Infinity).toBe(0);
    expect(-Infinity & Infinity).toBe(0);
});
