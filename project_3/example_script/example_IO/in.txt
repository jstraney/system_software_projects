const foo = 11, lol = 12;
var x, y;
begin
  y := 100;
  x := 10;
  while x < y do 
    x := x * 2;
  if y = 100 then
    y := y + lol;
  if odd y then
    x := x + foo;
end.
/*
program will double x until it is equal
to or greater than y. if y was odd, add 1 to x
*/
