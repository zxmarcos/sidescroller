-- samus test
NAME = "samus"
ANIM_FILE = "samus.air"
SPRITE_FILE = "samus.sff"
WALK_FACTOR = 2
VEL_MAX_Y_DOWN = 10
VEL_MAX_Y_UP = -10

-- nosso estado atual
xstate = nil
-- contador de tempo do estado atual
xtime = 0


function change_state(func)
	xstate = func
	xtime = 0
end

function invert_facing()
	if IsFacing() == 1 then
		SetFacing(0)
	else
		SetFacing(1)
	end
end

function air_control()
		-- controlar no ar
	if Input("foward") == 1 then
		AddVelX(0.05)
	elseif Input("back") == 1 then
		AddVelX(-0.05)
	end
end

-- *********************************************************************************
-- ********************************* NORMAL STATES *********************************
-- *********************************************************************************
-- stand state
function stand()
	if xtime == 1 then
		ChangeAnim(0)
	end
	if IsGrounded() == 0 then
		change_state(falling)
		return
	end
	-- sempre zeramos a velocidade de Y
	SetVelY(0)
	
	-- andar
	set_walking = function()
		change_state(walk)
		SetVelX(WALK_FACTOR)
	end
		
	if Input("foward") == 1 then
		set_walking()
	elseif Input("back") == 1 then
		set_walking()
		-- precisamos inverter nosso personagem
		invert_facing()
	elseif Input("a") == 1 then
		change_state(jump_start)
	elseif Input("down") == 1 then
		change_state(stand_to_crouch)
	end
end

-- andando
function walk()
	if xtime == 1 then
		ChangeAnim(20)
	end
	if IsGrounded() == 0 then
		change_state(falling)
		return
	end
	-- soltou o direcional
	if Input("^foward") == 1 then
		change_state(stand)
		SetVelX(0)
	elseif Input("a") == 1 then
		change_state(jump_start)
	end
end

-- começa a pular
function jump_start()
	if xtime == 1 then
		SetVelY(-7.0)
		ChangeAnim(40)
	elseif xtime == 2 then
		change_state(jump_raising)
	end
end

function wall_jump_check()
	if IsFaced() == 1 and (GetAnim() == 42 or GetAnim() == 43) then
		if (Input("back") == 1) and (Input("a") == 1) then
			change_state(wall_jumping)
		end
	end
end

function jump_raising()
	if xtime == 1 then
		if GetVelX() > 0 then
			if IsFacing() == 1 then
				ChangeAnim(43)
			else
				ChangeAnim(42)
			end
		else
			ChangeAnim(41)
		end
	end
	AddVelY(0.3)
	
	-- controlar no ar
	air_control()
	
	wall_jump_check()
	
	if GetVelY() > 0 or IsHeaded() == 1 then
		-- bateu a cabeça em algum sólido? :D
		if IsHeaded() == 1 then
			SetVelY(0)
		end
		change_state(falling)
	end
end

function falling()
	air_control()
	--	if xtime == 1 then
	--	SetVelX(GetVelX()*0.5)
	--end
	-- chegamos ao chão
	if IsGrounded() == 1 then
		SetVelY(0.0)
		SetVelX(0.0)
		-- cálculo para não ficarmos dentro do tile
		local y = GetY();
		y = y - (y % 16)
		SetY(y)
		change_state(stand)
	else
		AddVelY(0.3)
		wall_jump_check()
	end
end

local wj_save_x = 0
function wall_jumping()
	if xtime == 1 then
		invert_facing()
		wj_save_x = GetVelX()
		SetVelX(0)
		SetVelY(0)
		ChangeAnim(60)
	elseif xtime == 5 then
		SetVelX(wj_save_x)
		SetVelY(-7.0)
		change_state(jump_raising)
	end
end

function stand_to_crouch()
	if xtime == 1 then
		ChangeAnim(10)
	elseif xtime == 3 then
		change_state(crouch)
	end
end

function crouch()
	if xtime == 1 then
		ChangeAnim(11)
	elseif Input("up") == 1 then
		change_state(crouch_to_stand)
	elseif Input("down") == 1 then
		change_state(crouch_to_morphball)
	end
end

function crouch_to_stand()
	if xtime == 1 then
		ChangeAnim(10)
	elseif xtime == 3 then
		change_state(stand)
	end
end

-- *********************************************************************************
-- ****************************** MORPHBALL STATES *********************************
-- *********************************************************************************
function crouch_to_morphball()
	if xtime == 1 then
		ChangeAnim(160)
	elseif xtime == 4 then
		change_state(morphball_stand)
	end
end

function morphball_stand()
	if xtime == 1 then
		ChangeAnim(161)
	elseif Input("up") == 1 then
		change_state(crouch_to_stand)
	elseif Input("foward") == 1 then
		change_state(morphball_sliding)
	elseif Input("back") == 1 then
		change_state(morphball_sliding)
		invert_facing()
	elseif Input("a") == 1 then
		change_state(morphball_jump_start)
	end
	if IsGrounded() == 0 then
		change_state(morphball_falling)
		return
	end
end	

function morphball_to_crouch()
	if xtime == 1 then
		ChangeAnim(162)
	elseif xtime == 4 then
		change_state(crouch)
	end
end

function morphball_sliding()
	if xtime == 1 then
		SetVelX(WALK_FACTOR)
	elseif Input("^foward") == 1 then
		change_state(morphball_stand)
		SetVelX(0)
	elseif Input("a") == 1 then
		change_state(morphball_jump_start)
	end
	if IsGrounded() == 0 then
		change_state(morphball_falling)
		return
	end
end

function morphball_falling()

	-- chegamos ao chão
	if IsGrounded() == 1 then
		SetVelY(0.0)
		SetVelX(0.0)
		-- cálculo para não ficarmos dentro do tile
		local y = GetY();
		y = y - (y % 16)
		SetY(y)
		change_state(morphball_stand)
	else
		SetVelY(GetVelY() + 0.3)
	end
end

-- começa a pular
function morphball_jump_start()
	if xtime == 1 then
		SetVelY(-7.0)
	elseif xtime == 2 then
		change_state(morphball_jump_raising)
	end
end

function morphball_jump_raising()
	SetVelY(GetVelY() + 0.3)
	-- controlar no ar
	air_control()
	if GetVelY() > 0 or IsHeaded() == 1 then
		-- bateu a cabeça em algum sólido? :D
		if IsHeaded() == 1 then
			SetVelY(0)
		end
		change_state(morphball_falling)
	end
end

-- *********************************************************************************
-- ******************************                  *********************************
-- *********************************************************************************

-- update entity
function update()
	-- verifica primeira chamada
	if xstate == nil then
		xstate = stand
		--SetX(51)
		--SetY(236)
		
	end
	xstate()
	xtime = xtime + 1
	
	-- atualiza a posição
	
	local x, y, vx, vy = GetX(), GetY(), GetVelX(), GetVelY()
	if vy > 0 then
		if vy > VEL_MAX_Y_DOWN then
			vy = VEL_MAX_Y_DOWN
			SetVelY(vy)
		end
	elseif vy < 0 then
		if vy < VEL_MAX_Y_UP then
			vy = VEL_MAX_Y_UP
			SetVelY(vy)
		end
	end
	
	if IsFaced() == 0 then
		if IsFacing() == 1 then
			x = x - vx
		else
			x = x + vx
		end
		SetX(x)
	end
	y = y + vy
	SetY(y)
end
