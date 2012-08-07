<?

$f = fopen("Q4.res", "r");
$f1 = fopen("Q4_list.res", "r");

$num = array();
$res = array();

$i = 0;
while (!feof($f))
{
	$str = fgets($f);
	if (strpos($str, "TOOPOLY") === FALSE)
	{
//		echo "i = $i; ";
		$res[$i] = array(); 
		$res[$i] = explode(" " , $str);
		if (@count($res[$i]) == 1630)
		{
//			echo "i = $i; " . @count($res[$i]) . "; ";
			$num[$i] = array_shift($res[$i]);
			$num[$i] = substr($num[$i], 1, strlen($num[$i])-2);
//			echo $num[$i] . "; " . @count($res[$i]) . "\n";
			++$i;
		}
		else array_pop($res);
	}
}

echo "DONE\n";

$n = array();
while(!feof($f1))
{
	$n[] = fgets($f1);
}

//echo "5: " .$n[5]."\n20: ".$n[20]."\n1350: " .$n[1350]."\n";

for ($i = 0; $i < @count($res); ++$i)
{
	for ($j = 0; $j < @count($res[$i]); ++$j)
	{
//		if ($i == $j && $res[$i][$j] != 0.) echo "WHF?!\n";
		$res[$i][$j] = $res[$i][$j]/($n[$num[$i]]);
		if ($res[$i][$j] == 0) $res[$i][$j] = 0.; 
	}
}

$Q0 = 0;
$Q = 5; //@count($res)
for ($i = $Q0; $i < $Q; ++$i)
{
//	echo $num[$i]." ";
	for ($j = $Q0; $j < $Q; ++$j)
	{
		printf("%4f ", $res[$i][$j]);// . " ";
	}
	echo "\n";
}

echo "DONE 1;". @count($res) ."\n";

//exit;

$vec = array();

$RN = @count($num);
$RNi = @count($res[0]);

echo "RNi = " . @count($res[0]) . "\n";

for ($i = 0; $i < $RN; ++$i)
{
//	echo $i . "\n";
	$vec[$i] = array();
	for ($j = $i+1; $j < $RN; ++$j)
	{
//		echo $i . "; $j; $RNi; ". $RN ."\n";
		$vec[$i][$j] = 0.;
		for ($k = 0; $k < @count($res[$i]); ++$k)
		{			
			if ($k != $num[$i] && $k != $num[$j])
			{
				$vec[$i][$j] += ($res[$i][$k]-$res[$j][$k])*($res[$i][$k]-$res[$j][$k]);
				//echo $vec[$i][$j] ." ";
//				$t = $res[$i][$k]-$res[$j][$k];
//				echo "$t\n";
			}
//			echo $vec[$i][$j] . " ";
		}
//		echo @count($res[$i]). "; ". $k . "\n";
//		echo $vec[$i][$j] ." ";
		$vec[$i][$j] = sqrt($vec[$i][$j]);
		$vec[$j][$i] = $vec[$i][$j];
	}
	$vec[$i][$i] = 0.;
//	echo "\n";
}

echo "DONE 2\n";

//exit;

$VN = @count($vec);


$RN = 10;

for ($i = 0; $i < $RN; ++$i)
{
	echo $num[$i]." ";
}
echo "\n";

for ($i = 0; $i < $RN; ++$i)
{
	echo $num[$i]." ";
	for ($j = 0; $j < $RN; ++$j)
	{
		echo $vec[$i][$j] . " ";//. "; i = $i; j = $j;\n";
	}
	echo "\n";
}

echo "DONE 3\n";


?>
