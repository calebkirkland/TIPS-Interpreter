PROGRAM LOOPEX;
VAR
  COUNT: INTEGER;
  MULTIPLE: INTEGER;
  BASE : INTEGER;
BEGIN
  COUNT := 1;
  MULTIPLE := 0;
  
  WRITE('ENTER THE BASE INTEGER');
  READ(BASE);

  WRITE('MULTIPLES');
  WHILE COUNT < 12
  BEGIN
    MULTIPLE := COUNT * BASE;
    WRITE(MULTIPLE);
    COUNT := COUNT + 1
  END

END
