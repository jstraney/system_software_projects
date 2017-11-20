/* CUSTOM TEST */
/* global x */
var x;

procedure add;
  var y;
  begin
    y := x + 5;
  end;

procedure maybesub;
  /* x overwrites scope of global x. default value of 0 */
  var x, y;
  begin
    x := x - y;
  end;

procedure nested;
  /* this y should not be overwritten by y in add procedure */
  var y;
  begin;
    y := 1;
    while y > 0 do
      begin
        call add;
        y := y - 1
      end
  end;

begin
  x := 2;
  call add;
  call maybesub;
  call nested
end.
