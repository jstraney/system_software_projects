const foo = 200;
var x, z;
procedure a;
  var x;
  procedure b;
    var x;
    procedure c;
      var x;
      begin
        x := 20;
        write x
      end;
    begin
      call c 
    end;
  begin
    call b 
  end;

begin
  x := 5;
  call a
end.
