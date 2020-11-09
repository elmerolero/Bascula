CREATE TABLE productos( clave_producto integer not null primary key autoincrement, nombre_producto text( 35 ) not null ); CREATE TABLE empresas( clave_empresa integer not null primary key autoincrement, nombre_empresa text( 35 ) not null ); CREATE TABLE usuarios( nombre_usuario text( 25 ) not null primary key, contrasena text( 64 ) not null, sal text( 5 ) not null, nombre text( 35 ) not null, apellidos text( 35 ) not null, administrador integer not null ); CREATE TABLE registros_internos( folio integer not null primary key autoincrement, fecha text( 10 ) not null, tipo_registro integer not null, clave_empresa integer not null, clave_producto integer not null, numero_placas text( 7 ) not null, nombre_conductor text( 35 ) not null, hora_entrada text( 8 ), hora_salida text( 8 ), peso_bruto decimal, bruto_establecido integer, peso_tara decimal, tara_establecido integer, descuento decimal, descuento_establecido integer, descuento_permitido integer, peso_neto decimal, neto_establecido integer, observaciones text( 500 ), manual_activado integer, pendiente integer, nombre_basculista text( 70 ) not null, foreign key( clave_empresa ) references empresas(clave_empresa), foreign key( clave_producto ) references productos( clave_producto ) ); CREATE TABLE registros_publicos( folio integer not null primary key autoincrement, fecha text( 10 ) not null, tipo_viaje integer not null, clave_producto integer not null, numero_placas text( 7 ) not null, nombre_conductor text( 35 ) not null, hora_entrada text( 8 ), hora_salida text( 8 ), peso_bruto decimal, peso_tara decimal, peso_neto decimal, bruto_establecido integer, tara_establecido integer, neto_establecido integer, nombre_basculista text( 35 ), pendiente integer, entrada_manual integer );
